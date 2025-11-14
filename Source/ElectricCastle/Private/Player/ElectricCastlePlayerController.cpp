// Copyright Alien Shores


#include "Player/ElectricCastlePlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/ElectricCastleAbilitySystemComponent.h"
#include "Actor/MagicCircle.h"
#include "ElectricCastle/ElectricCastle.h"
#include "CommonInputSubsystem.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "Camera/CameraComponent.h"
#include "Character/ElectricCastlePlayerCharacter.h"
#include "Character/EnemyInterface.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Game/Subsystem/ElectricCastleGameDataSubsystem.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Input/ElectricCastleInputComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/Form/PlayerFormConfig.h"
#include "Tags/ElectricCastleGameplayTags.h"
#include "UI/HUD/ElectricCastleHUD.h"
#include "UI/Widget/DamageTextComponent.h"
#include "Player/ElectricCastlePlayerState.h"
#include "Character/ElectricCastlePlayerCharacter.h"

AElectricCastlePlayerController::AElectricCastlePlayerController()
{
	bReplicates = true;
}

void AElectricCastlePlayerController::BeginPlay()
{
	Super::BeginPlay();
	// Stop the game if AuraContext is not set
	check(AuraContext);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		GetLocalPlayer()
	))
	{
		Subsystem->AddMappingContext(AuraContext, 0);
	}
	if (UCommonInputSubsystem* CommonSubsystem = UCommonInputSubsystem::Get(GetLocalPlayer()))
	{
		CommonSubsystem->OnInputMethodChangedNative.AddUObject(
			this,
			&AElectricCastlePlayerController::OnInputTypeChange
		);
		OnInputTypeChange(CommonSubsystem->GetCurrentInputType());
	}
	if (IElectricCastleAbilitySystemInterface::IsAbilitySystemReady(GetPawn()))
	{
		GetAbilitySystemComponent()->RegisterGameplayTagEvent(
			FElectricCastleGameplayTags::Get().Effect_State_Aiming,
			EGameplayTagEventType::NewOrRemoved
		).AddUObject(
			this,
			&AElectricCastlePlayerController::OnEffectStateChanged_Aiming
		);
	}
	else
	{
		if (AElectricCastlePlayerCharacter* PlayerCharacter = Cast<AElectricCastlePlayerCharacter>(GetPawn()))
		{
			PlayerCharacter->GetOnAbilitySystemRegisteredDelegate().AddLambda(
				[this](UAbilitySystemComponent* LocalAbilitySystem)
				{
					LocalAbilitySystem->RegisterGameplayTagEvent(
						FElectricCastleGameplayTags::Get().Effect_State_Aiming,
						EGameplayTagEventType::NewOrRemoved
					).AddUObject(
						this,
						&AElectricCastlePlayerController::OnEffectStateChanged_Aiming
					);
				}
			);
		}
	}
}

void AElectricCastlePlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	CursorTrace();
	UpdateMagicCircleLocation();
}

void AElectricCastlePlayerController::GetLifetimeReplicatedProps(
	TArray<class FLifetimeProperty>& OutLifetimeProps
) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AElectricCastlePlayerController, InputType);
}

void AElectricCastlePlayerController::ShowMagicCircle(UMaterialInterface* DecalMaterial)
{
	if (!IsValid(MagicCircle))
	{
		MagicCircle = GetWorld()->SpawnActor<AMagicCircle>(MagicCircleClass);
		MagicCircle->SetDecalMaterial(DecalMaterial);
		SetShowMouseCursor(false);
	}
}

void AElectricCastlePlayerController::HideMagicCircle()
{
	if (IsValid(MagicCircle))
	{
		MagicCircle->Destroy();
		MagicCircle = nullptr;
		SetShowMouseCursor(true);
	}
}

void AElectricCastlePlayerController::ShowDamageNumber_Implementation(
	AActor* Target,
	const float DamageAmount,
	const bool IsBlockedHit,
	const bool IsCriticalHit
)
{
	if (IsValid(Target) && DamageTextComponentClass && IsLocalController())
	{
		UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(Target, DamageTextComponentClass);
		DamageText->RegisterComponent();
		DamageText->AttachToComponent(Target->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageText->SetDamageText(DamageAmount, IsBlockedHit, IsCriticalHit);
	}
}

void AElectricCastlePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UElectricCastleInputComponent* ElectricCastleInputComponent = CastChecked<UElectricCastleInputComponent>(
		InputComponent
	);
	ElectricCastleInputComponent->BindAbilityActions(
		InputConfig,
		this,
		&AElectricCastlePlayerController::AbilityInputTagPressed,
		&AElectricCastlePlayerController::AbilityInputTagReleased,
		&AElectricCastlePlayerController::AbilityInputTagHeld
	);
	ElectricCastleInputComponent->BindAction(
		MoveAction,
		ETriggerEvent::Triggered,
		this,
		&AElectricCastlePlayerController::Move
	);
	ElectricCastleInputComponent->BindAction(
		AimAction,
		ETriggerEvent::Triggered,
		this,
		&AElectricCastlePlayerController::Aim
	);
	ElectricCastleInputComponent->BindAction(
		FormChangeAction,
		ETriggerEvent::Triggered,
		this,
		&AElectricCastlePlayerController::HandleFormChangeInputAction
	);
	ElectricCastleInputComponent->BindAction(
		FormWheelAction,
		ETriggerEvent::Started,
		this,
		&AElectricCastlePlayerController::ShowFormWheel
	);
	ElectricCastleInputComponent->BindAction(
		FormWheelAction,
		ETriggerEvent::Completed,
		this,
		&AElectricCastlePlayerController::HideFormWheel
	);
}

void AElectricCastlePlayerController::Move(const FInputActionValue& Value)
{
	const bool bCanMove = GetAbilitySystemComponent() && !GetAbilitySystemComponent()->HasMatchingGameplayTag(
		FElectricCastleGameplayTags::Get().Player_Block_Movement
	);

	const bool bCanRotate = GetAbilitySystemComponent() && !GetAbilitySystemComponent()->HasMatchingGameplayTag(
		FElectricCastleGameplayTags::Get().Player_Block_Rotation
	);

	if (ACharacter* ControlledPawn = GetPawn<ACharacter>())
	{
		const FVector2D InputAxisVector = Value.Get<FVector2D>();
		const FRotator Rotation = ControlledPawn->FindComponentByClass<UCameraComponent>()->GetComponentRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		UCharacterMovementComponent* CharacterMovement = ControlledPawn->GetCharacterMovement();
		if (bCanMove)
		{
			CharacterMovement->bOrientRotationToMovement = true;
			ControlledPawn->bUseControllerRotationYaw = false;
			ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
			ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
		}
		else if (bCanRotate)
		{
			CharacterMovement->bOrientRotationToMovement = false;
			ControlledPawn->bUseControllerRotationYaw = true;
			// Use horizontal input to rotate the pawn
			if (!FMath::IsNearlyZero(InputAxisVector.X))
			{
				const float RotationAmount = FMath::Clamp(
					InputAxisVector.X * GetWorld()->GetDeltaSeconds() * 90.f,
					AimClampMin,
					AimClampMax
				);
				ControlledPawn->AddControllerYawInput(RotationAmount);
			}
		}
	}
}

void AElectricCastlePlayerController::Aim(const FInputActionValue& InputActionValue)
{
	const FElectricCastleGameplayTags& GameplayTags = FElectricCastleGameplayTags::Get();
	const UElectricCastleAbilitySystemComponent* LocalAbilitySystemComponent = GetAbilitySystemComponent();
	if (!LocalAbilitySystemComponent)
	{
		return;
	}
	if (IsAiming() && !LocalAbilitySystemComponent->HasMatchingGameplayTag(GameplayTags.Player_Block_Rotation))
	{
		ACharacter* ControlledPawn = GetPawn<ACharacter>();
		if (UCharacterMovementComponent* CharacterMovement = ControlledPawn
			                                                     ? ControlledPawn->GetCharacterMovement()
			                                                     : nullptr)
		{
			const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
			CharacterMovement->bOrientRotationToMovement = false;
			ControlledPawn->bUseControllerRotationYaw = true;
			// Use horizontal input to rotate the pawn
			if (!FMath::IsNearlyZero(InputAxisVector.X))
			{
				const float RotationAmount = FMath::Clamp(
					InputAxisVector.X * GetWorld()->GetDeltaSeconds() * 90.f,
					AimClampMin,
					AimClampMax
				);
				ControlledPawn->AddControllerYawInput(RotationAmount);
			}
		}
	}
}

void AElectricCastlePlayerController::CursorTrace()
{
	if (GetAbilitySystemComponent() && GetAbilitySystemComponent()->HasMatchingGameplayTag(
		FElectricCastleGameplayTags::Get().Player_Block_CursorTrace
	))
	{
		HighlightContext.Clear();
		return;
	}
	if (InputType == EAuraInputMode::Gamepad || IsAiming())
	{
		CursorTrace_Gamepad();
	}
	else
	{
		CursorTrace_Mouse();
	}
}

void AElectricCastlePlayerController::CursorTrace_Mouse()
{
	const ECollisionChannel TraceChannel = IsValid(MagicCircle)
		                                       ? ECC_ExcludeCharacters
		                                       : ECC_Target;
	GetHitResultUnderCursor(TraceChannel, false, CursorHit);
	if (CursorHit.bBlockingHit)
	{
		HighlightContext.Track(CursorHit.GetActor());
		if (HighlightContext.HasCurrentTarget())
		{
			TargetingStatus = IEnemyInterface::IsEnemyActor(
				                  HighlightContext.CurrentActor
			                  )
				                  ? ETargetingStatus::TargetingEnemy
				                  : ETargetingStatus::TargetingOther;
		}
		else
		{
			TargetingStatus = ETargetingStatus::NotTargeting;
		}
	}
	else
	{
		HighlightContext.Clear();
		TargetingStatus = ETargetingStatus::NotTargeting;
	}
}

void AElectricCastlePlayerController::CursorTrace_Gamepad()
{
	FLineTraceParams Params;
	Params.TraceChannel = IsValid(MagicCircle)
		                      ? ECC_ExcludeCharacters
		                      : ECC_Target;
	if (AActor* HitActor = UElectricCastleAbilitySystemLibrary::FindHitByLineTrace(GetPawn(), Params))
	{
		HighlightContext.Track(HitActor);
		if (HighlightContext.HasCurrentTarget())
		{
			TargetingStatus = IEnemyInterface::IsEnemyActor(
				                  HighlightContext.CurrentActor
			                  )
				                  ? ETargetingStatus::TargetingEnemy
				                  : ETargetingStatus::TargetingOther;
		}
		else
		{
			TargetingStatus = ETargetingStatus::NotTargeting;
		}
	}
	else
	{
		HighlightContext.Clear();
		TargetingStatus = ETargetingStatus::NotTargeting;
	}
}

UElectricCastleAbilitySystemComponent* AElectricCastlePlayerController::GetAbilitySystemComponent()
{
	if (AbilitySystemComponent == nullptr)
	{
		AbilitySystemComponent = Cast<UElectricCastleAbilitySystemComponent>(
			UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>())
		);
	}
	return AbilitySystemComponent;
}

void AElectricCastlePlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (UElectricCastleAbilitySystemComponent* LocalAbilitySystem = GetAbilitySystemComponent())
	{
		LocalAbilitySystem->AbilityInputTagPressed(InputTag);
	}
}

void AElectricCastlePlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (UElectricCastleAbilitySystemComponent* LocalAbilitySystem = GetAbilitySystemComponent())
	{
		LocalAbilitySystem->AbilityInputTagHeld(InputTag);
	}
}

void AElectricCastlePlayerController::ShowFormWheel(const FInputActionValue& InputActionValue)
{
	OnFormWheelVisibilityChange.Broadcast(
		FOnPlayerFormWheelVisibilityChangePayload(
			this,
			GetPlayerState<AElectricCastlePlayerState>(),
			GetPawn<AElectricCastlePlayerCharacter>(),
			true
		)
	);
}

void AElectricCastlePlayerController::HideFormWheel(const FInputActionValue& InputActionValue)
{
	OnFormWheelVisibilityChange.Broadcast(
		FOnPlayerFormWheelVisibilityChangePayload(
			this,
			GetPlayerState<AElectricCastlePlayerState>(),
			GetPawn<AElectricCastlePlayerCharacter>(),
			false
		)
	);
}

void AElectricCastlePlayerController::HandleFormChangeInputAction(const FInputActionValue& InputActionValue)
{
	const int32 FormId = FMath::RoundToInt32(InputActionValue.Get<float>());
	if (const UElectricCastleGameDataSubsystem* GameData = UElectricCastleGameDataSubsystem::Get(this))
	{
		if (const UPlayerFormConfig* FormConfig = GameData->GetPlayerFormConfig())
		{
			const FPlayerFormConfigRow& FormRow = FormConfig->GetPlayerFormConfigRowByFormId(FormId);
			if (!FormRow.IsValid())
			{
				return;
			}
			if (UElectricCastleAbilitySystemComponent* LocalAbilitySystem = GetAbilitySystemComponent())
			{
				LocalAbilitySystem->TryActivateAbilitiesByTag(FormRow.FormAbilityTag.GetSingleTagContainer());
			}
		}
	}
}

void AElectricCastlePlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (UElectricCastleAbilitySystemComponent* LocalAbilitySystemComponent = GetAbilitySystemComponent())
	{
		LocalAbilitySystemComponent->AbilityInputTagReleased(InputTag);
	}
}

void AElectricCastlePlayerController::UpdateMagicCircleLocation() const
{
	if (IsValid(MagicCircle))
	{
		MagicCircle->SetActorLocation(CursorHit.ImpactPoint);
	}
}

bool AElectricCastlePlayerController::IsTargetingEnemy() const
{
	return TargetingStatus == ETargetingStatus::TargetingEnemy;
}

bool AElectricCastlePlayerController::IsTargetingOther() const
{
	return TargetingStatus == ETargetingStatus::TargetingOther;
}

bool AElectricCastlePlayerController::IsNotTargeting() const
{
	return TargetingStatus == ETargetingStatus::NotTargeting;
}

void AElectricCastlePlayerController::SetupInputMode()
{
	FInputModeGameAndUI InputModeData;
	bShowMouseCursor = IsInputTypeMouse();
	DefaultMouseCursor = EMouseCursor::Default;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(!IsInputTypeMouse());
	SetInputMode(InputModeData);
}

void AElectricCastlePlayerController::OnInputTypeChange(const ECommonInputType NewInputMode)
{
	UE_LOG(
		LogElectricCastle,
		Warning,
		TEXT("[%s] Changing InputType: %s"),
		*GetName(),
		*UEnum::GetValueAsString(NewInputMode)
	)
	switch (NewInputMode)
	{
	case ECommonInputType::MouseAndKeyboard:
		SetInputMode_KeyboardAndMouse_Server();
		break;
	case ECommonInputType::Gamepad:
	default:
		SetInputMode_Gamepad_Server();
		break;
	}
	SetupInputMode();
}

void AElectricCastlePlayerController::OnEffectStateChanged_Aiming(const FGameplayTag AimingTag, const int TagCount)
{
	if (TagCount > 0)
	{
		bShowMouseCursor = false;
		FInputModeGameOnly InputModeData;
		InputModeData.SetConsumeCaptureMouseDown(true);
		SetInputMode(InputModeData);
	}
	else
	{
		bShowMouseCursor = IsInputTypeMouse();
		SetupInputMode();
	}
}

bool AElectricCastlePlayerController::IsAiming()
{
	if (const UElectricCastleAbilitySystemComponent* LocalAbilitySystemComponent = GetAbilitySystemComponent())
	{
		return LocalAbilitySystemComponent->HasMatchingGameplayTag(
			FElectricCastleGameplayTags::Get().Effect_State_Aiming
		);
	}
	return false;
}

void AElectricCastlePlayerController::SetInputMode_KeyboardAndMouse_Server_Implementation()
{
	InputType = EAuraInputMode::MouseAndKeyboard;
}

void AElectricCastlePlayerController::SetInputMode_Gamepad_Server_Implementation()
{
	InputType = EAuraInputMode::Gamepad;
}
