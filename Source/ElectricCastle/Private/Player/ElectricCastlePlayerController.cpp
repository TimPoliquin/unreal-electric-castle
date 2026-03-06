// Copyright Alien Shores


#include "Player/ElectricCastlePlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/ElectricCastleAbilitySystemComponent.h"
#include "ElectricCastle/ElectricCastle.h"
#include "CommonInputSubsystem.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "Actor/Cinematic/CinematicHandlerComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/ElectricCastlePlayerCharacter.h"
#include "Character/EnemyInterface.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Game/Subsystem/ElectricCastleGameDataSubsystem.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Input/ElectricCastleInputComponent.h"
#include "Net/UnrealNetwork.h"
#include "Tags/ElectricCastleGameplayTags.h"
#include "UI/Widget/DamageTextComponent.h"
#include "Player/ElectricCastlePlayerState.h"
#include "Player/Aim/AimActorInterface.h"
#include "Player/Aim/AimController.h"
#include "Player/Form/PlayerFormPrimaryAsset.h"
#include "Player/SelectionWheel/SelectionWheelManagerComponent.h"

AElectricCastlePlayerController::AElectricCastlePlayerController()
{
	bReplicates = true;
	SelectionWheelManager = CreateDefaultSubobject<USelectionWheelManagerComponent>(TEXT("SelectionWheelManager"));
	CinematicHandlerComponent = CreateDefaultSubobject<UCinematicHandlerComponent>(TEXT("CinematicHandlerComponent"));
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
	if (UElectricCastleGameDataSubsystem* GameData = UElectricCastleGameDataSubsystem::Get(this))
	{
		if (!GameData->IsGameDataLoaded())
		{
			DisableInput(this);
			GameData->OnGameDataLoaded.AddUniqueDynamic(this, &AElectricCastlePlayerController::OnGameDataLoaded);
		}
	}
}

void AElectricCastlePlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	// CursorTrace();
	if (UAimController* AimController = GetAimController(GetPawn()))
	{
		FVector CameraLocation;
		FRotator CameraRotation;
		GetPlayerViewPoint(CameraLocation, CameraRotation);
		AimController->TraceForTarget(GetPawn()->GetActorLocation(), CameraLocation, CameraRotation);
	}
}

void AElectricCastlePlayerController::GetLifetimeReplicatedProps(
	TArray<class FLifetimeProperty>& OutLifetimeProps
) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AElectricCastlePlayerController, InputType);
}

void AElectricCastlePlayerController::ShowDamageNumber_Implementation(
	AActor* Target,
	const float DamageAmount,
	const EAttackMessageType AttackMessageType
)
{
	if (IsValid(Target) && DamageTextComponentClass && IsLocalController())
	{
		UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(Target, DamageTextComponentClass);
		DamageText->RegisterComponent();
		DamageText->AttachToComponent(Target->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageText->SetDamageText(DamageAmount, AttackMessageType);
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
		ETriggerEvent::Completed,
		this,
		&AElectricCastlePlayerController::MoveEnd);
	ElectricCastleInputComponent->BindAction(
		MoveAction,
		ETriggerEvent::Triggered,
		this,
		&AElectricCastlePlayerController::Move
	);
	if (LookAction)
	{
		ElectricCastleInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AElectricCastlePlayerController::Look);
	}
	if (JumpAction)
	{
		ElectricCastleInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AElectricCastlePlayerController::JumpStart);
		ElectricCastleInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AElectricCastlePlayerController::JumpEnd);
	}

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
	if (SelectionWheelManager)
	{
		SelectionWheelManager->SetupInputComponent(ElectricCastleInputComponent);
		SelectionWheelManager->OnSelectionWheelStateChanged.AddUniqueDynamic(this, &AElectricCastlePlayerController::HandleSelectionWheelStateChanged);
	}
}

void AElectricCastlePlayerController::OnAbilitySystemReady_Implementation(UElectricCastleAbilitySystemComponent* InAbilitySystemComponent)
{
	if (UAimController* AimController = GetAimController(GetPawn()))
	{
		AimController->HandleAbilitySystemReady(InAbilitySystemComponent);
	}
}

void AElectricCastlePlayerController::OnGameDataLoaded_Implementation()
{
	EnableInput(this);
	if (UElectricCastleAbilitySystemComponent* LocalAbilitySystem = GetAbilitySystemComponent())
	{
		if (LocalAbilitySystem->HasFiredOnAbilitiesGivenDelegate())
		{
			OnAbilitySystemReady(LocalAbilitySystem);
		}
		else
		{
			LocalAbilitySystem->OnAbilitiesGivenDelegate.AddLambda([this]()
			{
				OnAbilitySystemReady(GetAbilitySystemComponent());
			});
		}
	}
}

void AElectricCastlePlayerController::Move(const FInputActionValue& Value)
{
	const bool bCanMove = GetAbilitySystemComponent()
		                      ? !GetAbilitySystemComponent()->HasMatchingGameplayTag(
			                      FElectricCastleGameplayTags::Get().Player_Block_Movement
		                      )
		                      : true;

	if (!bCanMove)
	{
		if (MovementEffectHandle.IsValid())
		{
			UElectricCastleAbilitySystemLibrary::RemoveGameplayEffect(GetPawn(), MovementEffectHandle);
			MovementEffectHandle.Invalidate();
		}
		return;
	}

	if (ACharacter* ControlledPawn = GetPawn<ACharacter>())
	{
		if (MovementEffect && !MovementEffectHandle.IsValid())
		{
			MovementEffectHandle = UElectricCastleAbilitySystemLibrary::ApplyBasicGameplayEffect(GetPawn(), MovementEffect);
		}
		const FVector2D InputAxisVector = Value.Get<FVector2D>();
		const FRotator Rotation = ControlledPawn->FindComponentByClass<UCameraComponent>()->GetComponentRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		UCharacterMovementComponent* CharacterMovement = ControlledPawn->GetCharacterMovement();
		CharacterMovement->bOrientRotationToMovement = true;
		ControlledPawn->bUseControllerRotationYaw = false;
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AElectricCastlePlayerController::MoveEnd(const FInputActionValue& Value)
{
	if (MovementEffectHandle.IsValid())
	{
		UElectricCastleAbilitySystemLibrary::RemoveGameplayEffect(GetPawn(), MovementEffectHandle);
		MovementEffectHandle.Invalidate();
	}
}

void AElectricCastlePlayerController::Look(const FInputActionValue& InputActionValue)
{
	const bool bCanRotate = GetAbilitySystemComponent()
		                        ? !GetAbilitySystemComponent()->HasMatchingGameplayTag(
			                        FElectricCastleGameplayTags::Get().Player_Block_Rotation
		                        )
		                        : true;
	const bool bIsAiming = IsAiming();
	ACharacter* ControlledPawn = GetPawn<ACharacter>();
	UCharacterMovementComponent* CharacterMovement = ControlledPawn ? ControlledPawn->GetCharacterMovement() : nullptr;
	if (!ControlledPawn || !CharacterMovement)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s] No pawn or movement component found for Look action"), *GetName());
		return;
	}
	if (bCanRotate && bIsAiming)
	{
		CharacterMovement->bOrientRotationToMovement = false;
		ControlledPawn->bUseControllerRotationYaw = true;
	}
	else
	{
		CharacterMovement->bOrientRotationToMovement = true;
		ControlledPawn->bUseControllerRotationYaw = false;
	}
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	ControlledPawn->AddControllerYawInput(bIsAiming ? FMath::Clamp(InputAxisVector.X, AimClampMin, AimClampMax) : InputAxisVector.X);
	ControlledPawn->AddControllerPitchInput(bIsAiming ? FMath::Clamp(InputAxisVector.Y, AimClampMin, AimClampMax) : InputAxisVector.Y);
}

void AElectricCastlePlayerController::JumpStart(const FInputActionValue& InputActionValue)
{
	const FElectricCastleGameplayTags& GameplayTags = FElectricCastleGameplayTags::Get();
	const UElectricCastleAbilitySystemComponent* LocalAbilitySystemComponent = GetAbilitySystemComponent();
	if (!LocalAbilitySystemComponent)
	{
		return;
	}
	if (!LocalAbilitySystemComponent->HasMatchingGameplayTag(GameplayTags.Player_Block_Movement))
	{
		if (ACharacter* ControlledPawn = GetPawn<ACharacter>())
		{
			ControlledPawn->Jump();
		}
	}
}

void AElectricCastlePlayerController::JumpEnd(const FInputActionValue& InputActionValue)
{
	if (ACharacter* ControlledPawn = GetPawn<ACharacter>())
	{
		ControlledPawn->StopJumping();
	}
}

void AElectricCastlePlayerController::Aim_Rotation(const FInputActionValue& InputActionValue)
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
	FLineTraceParams Params;
	Params.TraceChannel = ECC_Target;
	// TODO - this needs to be updated to trace from the camera to the crosshair
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
		if (const UPlayerFormPrimaryAsset* FormConfig = GameData->GetPlayerFormConfigById(FormId))
		{
			if (!FormConfig->IsValid())
			{
				return;
			}
			if (UElectricCastleAbilitySystemComponent* LocalAbilitySystem = GetAbilitySystemComponent())
			{
				LocalAbilitySystem->TryActivateAbilitiesByTag(FormConfig->FormAbilityTag.GetSingleTagContainer());
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
	DefaultMouseCursor = EMouseCursor::Default;
	bShowMouseCursor = false;
	bEnableClickEvents = false;
	bEnableMouseOverEvents = false;
	FInputModeGameOnly InputModeData;
	InputModeData.SetConsumeCaptureMouseDown(false);
	SetInputMode(InputModeData);
}

FInputModeGameAndUI AElectricCastlePlayerController::BuildGameAndUIInputMode() const
{
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(SelectionWheelManager->IsSelectionWheelActive() ? EMouseLockMode::LockAlways : EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(true);
	return InputModeData;
}

void AElectricCastlePlayerController::GetMovementVectors(const AController* Controller, FVector& OutForward, FVector& OutRight)
{
	if (!Controller)
	{
		return;
	}
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get forward vector
	OutForward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	// get right vector 
	OutRight = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
}

USelectionWheelManagerComponent* AElectricCastlePlayerController::GetSelectionWheelManagerComponent_Implementation() const
{
	return SelectionWheelManager;
}

UAimController* AElectricCastlePlayerController::GetAimController_Implementation() const
{
	return GetAimController(GetPawn());
}

void AElectricCastlePlayerController::OnInputTypeChange(const ECommonInputType NewInputType)
{
	if (bDebug)
	{
		UE_LOG(
			LogElectricCastle,
			Warning,
			TEXT("[%s] Changing InputType: %s"),
			*GetName(),
			*UEnum::GetValueAsString(NewInputType)
		)
	}
	switch (NewInputType)
	{
	case ECommonInputType::MouseAndKeyboard:
		SetInputMode_KeyboardAndMouse_Server();
		break;
	case ECommonInputType::Gamepad:
	default:
		SetInputMode_Gamepad_Server();
		break;
	}
	if (SelectionWheelManager)
	{
		SelectionWheelManager->SetInputType(NewInputType);
	}
	SetupInputMode();
}

void AElectricCastlePlayerController::HandleSelectionWheelStateChanged(const FSelectionWheelStateChangedPayload& Payload)
{
	SetupInputMode();
}

bool AElectricCastlePlayerController::IsAiming() const
{
	if (const UAimController* AimController = GetAimController(GetPawn()))
	{
		return AimController->IsAiming();
	}
	return false;
}

bool AElectricCastlePlayerController::HasEffectiveGameplayTag(const FGameplayTag& Tag)
{
	const UElectricCastleAbilitySystemComponent* LocalAbilitySystemComponent = GetAbilitySystemComponent();
	if (!LocalAbilitySystemComponent)
	{
		return false;
	}
	return LocalAbilitySystemComponent->HasMatchingGameplayTag(Tag);
}

void AElectricCastlePlayerController::SetInputMode_KeyboardAndMouse_Server_Implementation()
{
	InputType = EAuraInputMode::MouseAndKeyboard;
}

void AElectricCastlePlayerController::SetInputMode_Gamepad_Server_Implementation()
{
	InputType = EAuraInputMode::Gamepad;
}
