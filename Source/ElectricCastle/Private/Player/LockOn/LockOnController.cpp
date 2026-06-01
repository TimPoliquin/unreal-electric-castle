// Copyright Alien Shores


#include "Player/LockOn/LockOnController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystemInterface.h"
#include "AbilitySystem/ElectricCastleAbilitySystemComponent.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "AbilitySystem/ElectricCastleAttributeSet.h"
#include "Actor/MotionWarping/MotionWarpingActor.h"
#include "Character/ElectricCastleEnemyCharacter.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Game/Subsystem/ElectricCastleGameDataSubsystem.h"
#include "Input/Utils/PlayerInputFunctionLibrary.h"
#include "Interaction/CombatInterface.h"
#include "Player/ElectricCastlePlayerController.h"
#include "Player/LockOn/LockOnFunctionLibrary.h"
#include "Tags/ElectricCastleGameplayTags.h"

const float ULockOnController::REDUCED_TICK = 1.f / 10.f;

ULockOnController::ULockOnController()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	// set tick interval to 10 fps
	SetComponentTickInterval(REDUCED_TICK);
	bAutoActivate = false;
}

void ULockOnController::HandleAbilitySystemReady(UElectricCastleAbilitySystemComponent* InAbilitySystemComponent)
{
	const FElectricCastleGameplayTags& GameplayTags = FElectricCastleGameplayTags::Get();
	InAbilitySystemComponent->RegisterGameplayTagEvent(GameplayTags.Effect_State_LockedOn, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ULockOnController::HandleTagChange_Effect_LockedOn);
	InAbilitySystemComponent->RegisterGameplayTagEvent(
		GameplayTags.Player_Block_LockOn,
		EGameplayTagEventType::NewOrRemoved
	).AddUObject(
		this,
		&ULockOnController::HandleTagChange_BlockLockOn
	);
	HandleTagChange_BlockLockOn(
		GameplayTags.Player_Block_LockOn,
		InAbilitySystemComponent->GetTagCount(GameplayTags.Player_Block_LockOn)
	);
	if (UElectricCastleGameDataSubsystem* GameDataSubsystem = UElectricCastleGameDataSubsystem::Get(GetWorld()))
	{
		if (GameDataSubsystem->IsGameDataLoaded())
		{
			SetComponentTickEnabled(true);
		}
		else
		{
			GameDataSubsystem->OnGameDataLoaded.AddUniqueDynamic(this, &ULockOnController::HandleGameDataLoaded);
		}
	}
}

void ULockOnController::SetPlayerController(AElectricCastlePlayerController* InPlayerController)
{
	PlayerController = InPlayerController;
	if (IsValid(InPlayerController))
	{
		if (UEnhancedInputComponent* InputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			InputComponent->BindAction(
				SwitchTargetAction,
				ETriggerEvent::Started,
				this,
				&ULockOnController::HandleSwitchTargetInput
			);
		}
	}
}

void ULockOnController::TickComponent(
	const float DeltaTime, const ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction
)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (GetCanLockOn())
	{
		UpdateLockOnTarget();
		UpdateControllerRotation(DeltaTime);
		UpdateWarpTarget();
	}
	if (ULockOnFunctionLibrary::IsTargetValid(TargetActor.Get()) && LockOnLevel == ELockOnLevel::Hard)
	{
		if (GetComponentTickInterval() > 0.f)
		{
			SetComponentTickInterval(0);
		}
	}
	else
	{
		SetComponentTickInterval(REDUCED_TICK);
	}
}

bool ULockOnController::IsHardLockedOn() const
{
	return LockOnLevel == ELockOnLevel::Hard;
}

bool ULockOnController::HasLockOnTarget() const
{
	return TargetActor.IsValid();
}

bool ULockOnController::GetCanLockOn() const
{
	switch (LockOnLevel)
	{
	case ELockOnLevel::Soft:
		return true;
	case ELockOnLevel::Hard:
		return bLockOnSupported && !bLockOnBlocked;
	default:
		return false;
	}
}

void ULockOnController::SetLockOnSupported(const bool bInLockOnSupported)
{
	bLockOnSupported = bInLockOnSupported;
}

void ULockOnController::HandleTagChange_Effect_LockedOn(FGameplayTag LockedOnTag, int Count)
{
	if (Count > 0 && LockOnLevel != ELockOnLevel::Hard)
	{
		ActivateHardLockOn();
	}
	else if (LockOnLevel == ELockOnLevel::Hard)
	{
		DeactivateHardLockOn();
	}
}

bool ULockOnController::ActivateHardLockOn()
{
	if (!PlayerController.IsValid())
	{
		UE_LOG(
			LogElectricCastle,
			Warning,
			TEXT("[%s:%s] No Player Controller assigned!"),
			*GetOwner()->GetName(),
			*GetName()
		)
		return false;
	}
	if (!GetCanLockOn())
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] Lock on blocked!"), *GetOwner()->GetName(), *GetName())
		return false;
	}
	if (ULockOnFunctionLibrary::IsTargetValid(TargetActor.Get()))
	{
		// don't switch targets if you already have one
	}
	else if (AActor* NewTarget = ULockOnFunctionLibrary::FindClosestTarget(PlayerController.Get(), MaxLockOnDistance, true, bDebug))
	{
		// find a new target
		EngageNewTarget(NewTarget);
	}
	if (ULockOnFunctionLibrary::IsTargetValid(TargetActor.Get()))
	{
		// enable full tick rate for hard lock-on
		UPlayerInputFunctionLibrary::AddInputMappingContext(PlayerController.Get(), LockOnContext);
		SetLockOnLevel(ELockOnLevel::Hard);
		return true;
	}
	return false;
}

void ULockOnController::DeactivateHardLockOn()
{
	if (LockOnLevel != ELockOnLevel::Hard)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] Attempting to deactivate hard lock on when already deactivated!"), *GetOwner()->GetName(), *GetName())
		return;
	}
	// set tick rate back to 10fps
	UPlayerInputFunctionLibrary::RemoveInputMappingContext(PlayerController.Get(), LockOnContext);
	SetLockOnLevel(ELockOnLevel::Soft);
}

AActor* ULockOnController::GetLockOnTarget() const
{
	return TargetActor.Get();
}

bool ULockOnController::ShouldUpdateTarget() const
{
	switch (LockOnLevel)
	{
	case ELockOnLevel::Soft:
		return true;
	case ELockOnLevel::Hard:
		if (!ULockOnFunctionLibrary::IsTargetValid(TargetActor.Get()))
		{
			// target is no longer valid
			return true;
		}
		if (FVector::DistSquared(GetOwner()->GetActorLocation(), TargetActor->GetActorLocation()) > MaxLockOnDistance * MaxLockOnDistance)
		{
			// target is out of range
			return true;
		}
		return false;
	default:
		return false;
	}
}

void ULockOnController::UpdateLockOnTarget()
{
	if (!ShouldUpdateTarget())
	{
		return;
	}
	if (!PlayerController.IsValid())
	{
		UE_LOG(
			LogElectricCastle,
			Error,
			TEXT("[%s:%s] No valid player controller set!"),
			*GetOwner()->GetName(),
			*GetName()
		)
		return;
	}
	if (bDebug)
	{
		UE_LOG(
			LogElectricCastle,
			Warning,
			TEXT("[%s:%s] Updating lock-on target"),
			*GetOwner()->GetName(),
			*GetName()
		)
	}
	if (AActor* NewTarget = ULockOnFunctionLibrary::FindClosestTarget(PlayerController.Get(), MaxLockOnDistance, true, bDebug))
	{
		if (!TargetActor.IsValid() || TargetActor.Get() != NewTarget)
		{
			DisengageCurrentTarget();
			EngageNewTarget(NewTarget);
		}
	}
	else if (LockOnLevel == ELockOnLevel::Hard)
	{
		DisengageCurrentTarget();
		DeactivateHardLockOn();
	}
	else
	{
		DisengageCurrentTarget();
	}
}

void ULockOnController::HandleTagChange_BlockLockOn(FGameplayTag LockOnTag, const int Count)
{
	bLockOnBlocked = Count > 0;
}

void ULockOnController::UpdateControllerRotation(const float DeltaTime)
{
	if (!TargetActor.IsValid())
	{
		return;
	}
	if (!PlayerController.IsValid())
	{
		return;
	}
	if (LockOnLevel != ELockOnLevel::Hard)
	{
		return;
	}
	const FRotator CurrentRotation = PlayerController->GetControlRotation();
	const FVector Direction = (TargetActor->GetActorLocation() - GetOwner()->GetActorLocation()).GetSafeNormal();
	FRotator LookAtRotation = Direction.Rotation();
	LookAtRotation.Pitch = FMath::FixedTurn(CurrentRotation.Pitch, LockOnPitchOverride, 180.f * DeltaTime);
	const FRotator NewRotation = FMath::RInterpTo(CurrentRotation, LookAtRotation, DeltaTime, LockOnRotationSpeed);
	PlayerController->SetControlRotation(NewRotation);
}

void ULockOnController::DisengageCurrentTarget()
{
	if (TargetActor.IsValid())
	{
		if (bDebug)
		{
			UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] Disengaging lock on from old target: %s"), *GetOwner()->GetName(), *GetName(), *TargetActor->GetName())
		}
		IHighlightActorInterface::Unhighlight(TargetActor.Get());
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(TargetActor.Get()))
		{
			CombatInterface->GetOnDeathDelegate().RemoveAll(this);
		}
		else
		{
			TargetActor->OnDestroyed.RemoveAll(this);
		}
		TargetActor.Reset();
		ICombatInterface::ClearFacingTarget(GetOwner());
		OnLockOnTargetChanged.Broadcast(FLockOnTargetPayload(nullptr, LockOnLevel));
	}
}

void ULockOnController::HandleTargetDestroyed(AActor* DestroyedActor)
{
	if (DestroyedActor == TargetActor)
	{
		DisengageCurrentTarget();
	}
}

void ULockOnController::EngageNewTarget(AActor* InTargetActor)
{
	// if the new actor is invalid, just clear the target actor
	if (!InTargetActor)
	{
		TargetActor.Reset();
		return;
	}
	if (bDebug)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] Engaging lock on on new target: %s"), *GetOwner()->GetName(), *GetName(), *InTargetActor->GetName())
	}
	TargetActor = InTargetActor;
	// highlight the new target
	IHighlightActorInterface::Highlight(InTargetActor);
	// add death/destroy event handlers
	if (ICombatInterface* CombatTarget = Cast<ICombatInterface>(TargetActor.Get()))
	{
		CombatTarget->GetOnDeathDelegate().AddUniqueDynamic(this, &ULockOnController::HandleTargetDestroyed);
	}
	else
	{
		TargetActor->OnDestroyed.AddUniqueDynamic(this, &ULockOnController::HandleTargetDestroyed);
	}
	OnLockOnTargetChanged.Broadcast(FLockOnTargetPayload(InTargetActor, LockOnLevel));
}


void ULockOnController::HandleSwitchTargetInput(const FInputActionValue& InputActionValue)
{
	const FVector2D RawInput = InputActionValue.Get<FVector2D>();
	FVector CameraLocation;
	FRotator CameraRotation;
	PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);
	if (AActor* NewTarget = ULockOnFunctionLibrary::SwitchTarget(PlayerController.Get(), TargetActor.Get(), RawInput, MaxLockOnDistance, true, bDebug))
	{
		DisengageCurrentTarget();
		EngageNewTarget(NewTarget);
	}
}

void ULockOnController::UpdateWarpTarget() const
{
	if (IMotionWarpingActor::IsMotionWarping(GetOwner()))
	{
		return;
	}
	if (TargetActor.IsValid())
	{
		const float MaxLungeDistance = UElectricCastleAbilitySystemLibrary::GetLungeDistance(GetOwner());
		FVector TargetLocation = TargetActor->GetActorLocation();
		if (FVector::DistSquared(GetOwner()->GetActorLocation(), TargetLocation) > (MaxLungeDistance * MaxLungeDistance))
		{
			const FVector Direction = (TargetLocation - GetOwner()->GetActorLocation()).GetSafeNormal();
			TargetLocation = GetOwner()->GetActorLocation() + MaxLungeDistance * Direction;
		}
		if (bDebug)
		{
			DrawDebugSphere(GetWorld(), TargetLocation + FVector::UpVector * 10.f, 20.f, 12, FColor::Red, false, 0, 0, 1);
		}
		ICombatInterface::UpdateFacingTarget(GetOwner(), TargetLocation);
	}
	else
	{
		ICombatInterface::ClearFacingTarget(GetOwner());
	}
}

void ULockOnController::SetLockOnLevel(const ELockOnLevel InLockOnLevel)
{
	LockOnLevel = InLockOnLevel;
	OnLockOnLevelChanged.Broadcast(FLockOnTargetPayload(TargetActor.Get(), LockOnLevel));
}

void ULockOnController::HandleGameDataLoaded()
{
	Activate(true);
}
