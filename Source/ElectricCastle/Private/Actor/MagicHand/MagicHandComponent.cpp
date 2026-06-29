// Copyright Alien Shores

#include "Actor/MagicHand/MagicHandComponent.h"
#include "Actor/MagicHand/MagicHandProjectile.h"
#include "Actor/MagicHand/Handler/TetherUpdateHandler.h"
#include "Actor/MagicHand/MagicHandPossessable.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Actor/MagicHand/MagicHandPossessorInterface.h"
#include "Actor/Significance/Component/ActorSignificanceComponent.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "GameFramework/Pawn.h"

UMagicHandComponent::UMagicHandComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PrePhysics;
}

void UMagicHandComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	PreviousOwnerLocation = Owner->GetActorLocation();
}

void UMagicHandComponent::TickComponent(
	float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction
)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	switch (CurrentState)
	{
	case EMagicHandState::Casting:
		// If the projectile destroyed itself (exceeded range or missed), reset.
		if (!IsValid(ActiveProjectile))
		{
			ActiveProjectile = nullptr;
			SetState(EMagicHandState::Idle);
		}
		break;

	case EMagicHandState::Tethered:
		TickTether();
		break;

	default:
		break;
	}
}

// ──────────────────────────────────────────────────────────────────────────────
// Public interface
// ──────────────────────────────────────────────────────────────────────────────

void UMagicHandComponent::ActivateMagicHand()
{
	if (CurrentState != EMagicHandState::Idle)
	{
		return;
	}

	AActor* Owner = GetOwner();
	if (!Owner || !ProjectileClass)
	{
		return;
	}

	FVector SpawnLocation = Owner->GetActorLocation();
	FRotator SpawnRotation = Owner->GetActorRotation();

	if (const APawn* OwnerPawn = Cast<APawn>(Owner))
	{
		if (const AController* Controller = OwnerPawn->GetController())
		{
			Controller->GetPlayerViewPoint(SpawnLocation, SpawnRotation);
		}
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Owner;
	SpawnParams.Instigator = Cast<APawn>(Owner);

	ActiveProjectile = Owner->GetWorld()->SpawnActor<AMagicHandProjectile>(
		ProjectileClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);

	if (IsValid(ActiveProjectile))
	{
		ActiveProjectile->Launch(SpawnRotation.Vector(), this);
		SetState(EMagicHandState::Casting);
	}
}

void UMagicHandComponent::ReleaseTether()
{
	if (CurrentState == EMagicHandState::Tethered || CurrentState == EMagicHandState::Casting)
	{
		EndPossession();
	}
}

FVector UMagicHandComponent::GetAnchorLocation() const
{
	if (ActiveProjectile)
	{
		return ActiveProjectile->GetActorLocation();
	}
	if (TargetActor)
	{
		return TargetActor->GetActorLocation();
	}
	return FVector::ZeroVector;
}

void UMagicHandComponent::NotifyProjectileHitPossessable(
	AActor* PossessableActor,
	AMagicHandProjectile* HittingProjectile
)
{
	if (CurrentState != EMagicHandState::Casting)
	{
		return;
	}

	if (!PossessableActor || !PossessableActor->Implements<UMagicHandPossessable>())
	{
		return;
	}

	if (!IMagicHandPossessable::Execute_CanBePossessedByMagicHand(PossessableActor))
	{
		return;
	}

	BeginPossession(PossessableActor, HittingProjectile);
}

void UMagicHandComponent::NotifyProjectileMissed(AMagicHandProjectile* InMagicHandProjectile)
{
	OnMissed.Broadcast();
	EndPossession();
}

FVector UMagicHandComponent::GetAllowedMovementInput(const FVector& DesiredInputVector) const
{
	if (!bContextValid || !IsValid(ActiveHandler) || !IsValid(TargetActor))
	{
		return DesiredInputVector;
	}

	return ActiveHandler->QueryAllowedMovement(LastContext, DesiredInputVector);
}

// ──────────────────────────────────────────────────────────────────────────────
// Private helpers
// ──────────────────────────────────────────────────────────────────────────────

void UMagicHandComponent::SetState(EMagicHandState NewState)
{
	CurrentState = NewState;
}

void UMagicHandComponent::BeginPossession(AActor* Actor, AMagicHandProjectile* FromProjectile)
{
	check(Actor);

	TargetActor = Actor;
	ActiveHandler = IMagicHandPossessable::Execute_GetTetherUpdateHandler(Actor);

	if (!IsValid(ActiveHandler))
	{
		UE_LOG(
			LogElectricCastle,
			Warning,
			TEXT("[MagicHandComponent] '%s' returned a null handler — possession aborted."),
			*Actor->GetName()
		);
		DestroyActiveProjectile();
		SetState(EMagicHandState::Idle);
		return;
	}

	IMagicHandPossessable::Execute_OnPossessionStarted(Actor, FMagicHandPossessEventPayload(GetOwner(), Actor, this));

	// Keep the projectile alive; it will attach itself to the target as a visual anchor.
	ActiveProjectile = FromProjectile;
	if (IsValid(ActiveProjectile))
	{
		ActiveProjectile->EnterTetherAnchorMode(Actor);
	}

	if (TetherNiagaraSystem && GetOwner())
	{
		USceneComponent* AttachComponent = GetOwner()->GetRootComponent();
		FName AttachBone = NAME_None;
		if (USceneComponent* PossessorAttachComponent = IMagicHandPossessorInterface::GetMagicHandAttachComponent(GetOwner(), AttachBone))
		{
			AttachComponent = PossessorAttachComponent;
		}
		TetherNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			TetherNiagaraSystem,
			AttachComponent,
			AttachBone,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget,
			true
		);
	}

	if (AActor* Owner = GetOwner())
	{
		PreviousOwnerLocation = Owner->GetActorLocation();
	}

	SetState(EMagicHandState::Tethered);
	OnPossessionChanged.Broadcast(Actor, true);
}

void UMagicHandComponent::EndPossession()
{
	AActor* PreviousTarget = TargetActor;

	if (IsValid(TargetActor) && TargetActor->Implements<UMagicHandPossessable>())
	{
		IMagicHandPossessable::Execute_OnPossessionEnded(TargetActor, FMagicHandPossessEventPayload(GetOwner(), TargetActor, this));
	}

	DestroyActiveProjectile();

	if (IsValid(TetherNiagaraComponent))
	{
		TetherNiagaraComponent->DeactivateImmediate();
		TetherNiagaraComponent->DestroyComponent();
		TetherNiagaraComponent = nullptr;
	}

	TargetActor = nullptr;
	ActiveHandler = nullptr;
	bContextValid = false;

	SetState(EMagicHandState::Idle);

	if (IsValid(PreviousTarget))
	{
		OnPossessionChanged.Broadcast(PreviousTarget, false);
	}
}

void UMagicHandComponent::TickTether()
{
	AActor* Owner = GetOwner();
	if (!Owner || !IsValid(TargetActor) || !IsValid(ActiveHandler))
	{
		EndPossession();
		return;
	}

	const FVector CurrentOwnerLocation = Owner->GetActorLocation();

	// Build context. BaseOwnerDelta is one frame lagged (this frame's player position
	// minus last frame's). This is consistent with the reference TrackUpdateTetherHandler.
	FTetherHandlerContext Context;
	Context.OwnerActor = Owner;
	Context.TargetActor = TargetActor;
	Context.MaxTetherLength = MaxTetherLength;
	Context.TetherLengthPadding = TetherLengthPadding;
	Context.PreviousOwnerLocation = PreviousOwnerLocation;
	Context.BaseOwnerDelta = CurrentOwnerLocation - PreviousOwnerLocation;

	// Cache before UpdateTetherPhysics so GetAllowedMovementInput (called later this
	// same tick by CMC::ConsumeInputVector) sees the up-to-date target position.
	LastContext = Context;
	bContextValid = true;

	const FTetherHandlerResult Result = ActiveHandler->UpdateTetherPhysics(Context);

	PreviousOwnerLocation = CurrentOwnerLocation;

	if (Result.bLimitExceeded)
	{
		UE_LOG(
			LogElectricCastle,
			Log,
			TEXT("[MagicHandComponent] Tether limit exceeded for '%s' — releasing."),
			*TargetActor->GetName()
		);
		EndPossession();
		return;
	}

	UpdateTetherVisual();
}

void UMagicHandComponent::UpdateTetherVisual()
{
	if (!IsValid(TetherNiagaraComponent) || !GetOwner() || !IsValid(TargetActor) || !IsValid(ActiveProjectile))
	{
		return;
	}

	// FVector OwnerLoc;
	// TetherNiagaraComponent->SetVectorParameter(BeamStartParamName, OwnerLoc);

	const FVector TargetLoc = IsValid(ActiveProjectile) ? ActiveProjectile->GetActorLocation() : TargetActor->GetActorLocation();
	TetherNiagaraComponent->SetVectorParameter(BeamEndParamName, TargetLoc);
}

void UMagicHandComponent::DestroyActiveProjectile()
{
	if (IsValid(ActiveProjectile))
	{
		ActiveProjectile->Destroy();
		ActiveProjectile = nullptr;
	}
}
