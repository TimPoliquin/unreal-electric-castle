// Copyright Alien Shores


#include "Player/LockOn/LockOnController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystemInterface.h"
#include "AbilitySystem/ElectricCastleAbilitySystemComponent.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "Actor/MotionWarping/MotionWarpingActor.h"
#include "Character/ElectricCastleEnemyCharacter.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Input/Utils/PlayerInputFunctionLibrary.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/ElectricCastlePlayerController.h"
#include "Player/LockOn/LockOnFunctionLibrary.h"
#include "Tags/ElectricCastleGameplayTags.h"


// Sets default values for this component's properties
ULockOnController::ULockOnController()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void ULockOnController::HandleAbilitySystemReady(UElectricCastleAbilitySystemComponent* InAbilitySystemComponent)
{
	const FElectricCastleGameplayTags& GameplayTags = FElectricCastleGameplayTags::Get();
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
	if (bLockOnActivated && GetCanLockOn())
	{
		UpdateLockOnTarget();
		UpdateControllerRotation(DeltaTime);
	}
	else if (bLockOnActivated)
	{
		DeactivateLockOn();
	}
}

bool ULockOnController::IsLockedOn() const
{
	return bLockOnActivated;
}

bool ULockOnController::GetCanLockOn() const
{
	return bLockOnSupported && !bLockOnBlocked;
}

void ULockOnController::SetLockOnSupported(const bool bInLockOnSupported)
{
	bLockOnSupported = bInLockOnSupported;
}

bool ULockOnController::ActivateLockOn()
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
	if (bLockOnActivated)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] Lock on already active!"), *GetOwner()->GetName(), *GetName())
		return false;
	}
	if (AActor* NewTarget = ULockOnFunctionLibrary::FindClosestTarget(GetOwner(), MaxLockOnDistance, MaxLockOnDistance, true))
	{
		EngageNewTarget(NewTarget);
		UPlayerInputFunctionLibrary::AddInputMappingContext(PlayerController.Get(), LockOnContext);
		bLockOnActivated = true;
		return true;
	}
	return false;
}

void ULockOnController::DeactivateLockOn()
{
	bLockOnActivated = false;
	DisengageCurrentTarget();
	UPlayerInputFunctionLibrary::RemoveInputMappingContext(PlayerController.Get(), LockOnContext);
	OnLockOnRelease.Broadcast();
}

bool ULockOnController::ShouldUpdateTarget() const
{
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
}

TArray<AActor*> ULockOnController::FindPotentialTargets() const
{
	TArray<AActor*> PotentialTargets;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AElectricCastleEnemyCharacter::StaticClass(), PotentialTargets);
	PotentialTargets = PotentialTargets.FilterByPredicate(
		[](const AActor* Actor)
		{
			if (!IsValid(Actor))
			{
				return false;
			}
			if (Actor->Implements<UCombatInterface>())
			{
				return ICombatInterface::IsAlive(Actor);
			}
			return true;
		}
	);
	if (bDebug)
	{
		UE_LOG(
			LogElectricCastle,
			Warning,
			TEXT("[%s:%s] Found %d potential targets"),
			*GetOwner()->GetName(),
			*GetName(),
			PotentialTargets.Num()
		)
	}
	return PotentialTargets;
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
	// first, disengage from the current target
	DisengageCurrentTarget();
	if (bDebug)
	{
		UE_LOG(
			LogElectricCastle,
			Warning,
			TEXT("[%s:%s] Target lost - update to new target"),
			*GetOwner()->GetName(),
			*GetName()
		)
	}
	if (AActor* NewTarget = ULockOnFunctionLibrary::FindClosestTarget(GetOwner(), MaxLockOnDistance, MaxLockOnDistance, true))
	{
		EngageNewTarget(NewTarget);
	}
	else
	{
		DeactivateLockOn();
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
	const FRotator CurrentRotation = PlayerController->GetControlRotation();
	const FVector Direction = (TargetActor->GetActorLocation() - GetOwner()->GetActorLocation()).GetSafeNormal();
	FRotator LookAtRotation = Direction.Rotation();
	LookAtRotation.Pitch = FMath::FixedTurn(CurrentRotation.Pitch, LockOnPitchOverride, 180.f * DeltaTime);
	const FRotator NewRotation = FMath::RInterpTo(CurrentRotation, LookAtRotation, DeltaTime, LockOnRotationSpeed);
	// NewRotation.Pitch = FMath::ClampAngle(NewRotation.Pitch, -80, 10.f);
	PlayerController->SetControlRotation(NewRotation);
	UpdateWarpTarget();
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
	}
	TargetActor.Reset();
	ICombatInterface::ClearFacingTarget(GetOwner());
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
	OnLockOnTarget.Broadcast(FLockOnTargetPayload(InTargetActor));
}


void ULockOnController::HandleSwitchTargetInput(const FInputActionValue& InputActionValue)
{
	const FVector2D RawInput = InputActionValue.Get<FVector2D>();
	FVector CameraLocation;
	FRotator CameraRotation;
	PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);
	if (AActor* NewTarget = ULockOnFunctionLibrary::SwitchTarget(GetOwner(), TargetActor.Get(), RawInput, CameraRotation, MaxLockOnDistance, MaxLockOnDistance, true, bDebug))
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
		FVector TargetLocation = TargetActor->GetActorLocation();
		if (FVector::DistSquared(GetOwner()->GetActorLocation(), TargetLocation) > MaxWarpingDistance * MaxWarpingDistance)
		{
			const FVector Direction = (TargetLocation - GetOwner()->GetActorLocation()).GetSafeNormal();
			TargetLocation = GetOwner()->GetActorLocation() + MaxWarpingDistance * Direction;
		}
		ICombatInterface::UpdateFacingTarget(GetOwner(), TargetLocation);
	}
	else
	{
		ICombatInterface::ClearFacingTarget(GetOwner());
	}
}
