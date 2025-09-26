// Copyright Alien Shores


#include "Player/ElectricCastlePlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/ElectricCastleAbilitySystemComponent.h"
#include "Actor/MagicCircle.h"
#include "ElectricCastle/ElectricCastle.h"
#include "CommonInputSubsystem.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Camera/CameraComponent.h"
#include "Character/EnemyInterface.h"
#include "Input/ElectricCastleInputComponent.h"
#include "Tags/ElectricCastleGameplayTags.h"
#include "UI/Widget/DamageTextComponent.h"

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
		CommonSubsystem->OnInputMethodChangedNative.AddUObject(this, &AElectricCastlePlayerController::InitializeInputMode);
		InitializeInputMode(CommonSubsystem->GetCurrentInputType());
	}
}

void AElectricCastlePlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	CursorTrace();
	UpdateMagicCircleLocation();
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
	UElectricCastleInputComponent* AuraInputComponent = CastChecked<UElectricCastleInputComponent>(InputComponent);
	AuraInputComponent->BindAbilityActions(
		InputConfig,
		this,
		&AElectricCastlePlayerController::AbilityInputTagPressed,
		&AElectricCastlePlayerController::AbilityInputTagReleased,
		&AElectricCastlePlayerController::AbilityInputTagHeld
	);
	AuraInputComponent->BindAction(
		MoveAction,
		ETriggerEvent::Triggered,
		this,
		&AElectricCastlePlayerController::Move
	);
}

void AElectricCastlePlayerController::Move(const FInputActionValue& Value)
{
	if (GetAbilitySystemComponent() && GetAbilitySystemComponent()->HasMatchingGameplayTag(
		FElectricCastleGameplayTags::Get().Player_Block_Movement
	))
	{
		return;
	}
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		const FVector2D InputAxisVector = Value.Get<FVector2D>();
		// const FRotator Rotation = GetControlRotation();
		const FRotator Rotation = ControlledPawn->FindComponentByClass<UCameraComponent>()->
		                                          GetComponentRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
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
	const ECollisionChannel TraceChannel = IsValid(MagicCircle)
		                                       ? ECC_ExcludeCharacters
		                                       : ECC_Target;
	GetHitResultUnderCursor(TraceChannel, false, CursorHit);
	if (CursorHit.bBlockingHit)
	{
		HighlightContext.Track(CursorHit.GetActor());
		if (HighlightContext.HasCurrentTarget())
		{
			TargetingStatus = HighlightContext.HasCurrentTarget() && IEnemyInterface::IsEnemyActor(
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

void AElectricCastlePlayerController::InitializeInputMode(const ECommonInputType NewInputMode)
{
	FInputModeGameAndUI InputModeData;
	switch (NewInputMode)
	{
	case ECommonInputType::MouseAndKeyboard:
		InputType = EAuraInputMode::MouseAndKeyboard;
		bShowMouseCursor = true;
		DefaultMouseCursor = EMouseCursor::Default;
		InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputModeData.SetHideCursorDuringCapture(false);
		SetInputMode(InputModeData);
		break;
	case ECommonInputType::Gamepad:
	default:
		InputType = EAuraInputMode::Gamepad;
		bShowMouseCursor = false;
		DefaultMouseCursor = EMouseCursor::Default;
		InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputModeData.SetHideCursorDuringCapture(true);
		SetInputMode(InputModeData);
		break;
	}
}
