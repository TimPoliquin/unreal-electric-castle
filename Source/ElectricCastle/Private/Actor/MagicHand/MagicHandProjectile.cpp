// Copyright Alien Shores

#include "Actor/MagicHand/MagicHandProjectile.h"
#include "Actor/MagicHand/MagicHandComponent.h"
#include "Actor/MagicHand/MagicHandPossessable.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

AMagicHandProjectile::AMagicHandProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->InitSphereRadius(15.f);
	CollisionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	RootComponent = CollisionSphere;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = ProjectileSpeed;
	ProjectileMovement->MaxSpeed = ProjectileSpeed;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	ProjectileMovement->bAutoActivate = false;
}

void AMagicHandProjectile::BeginPlay()
{
	Super::BeginPlay();

	LaunchLocation = GetActorLocation();

	CollisionSphere->OnComponentBeginOverlap.AddDynamic(
		this,
		&AMagicHandProjectile::OnSphereBeginOverlap
	);
}

void AMagicHandProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bInAnchorMode)
	{
		SetActorTickEnabled(false);
		return;
	}

	if (FVector::Distance(LaunchLocation, GetActorLocation()) >= MaxTravelDistance)
	{
		OwnerMagicHandComponent->NotifyProjectileMissed(this);
	}
}

void AMagicHandProjectile::Launch(const FVector& Direction, UMagicHandComponent* InOwnerComponent)
{
	OwnerMagicHandComponent = InOwnerComponent;
	LaunchLocation = GetActorLocation();

	ProjectileMovement->InitialSpeed = ProjectileSpeed;
	ProjectileMovement->MaxSpeed = ProjectileSpeed;
	ProjectileMovement->Velocity = Direction.GetSafeNormal() * ProjectileSpeed;
	ProjectileMovement->Activate();

	if (TravelEffectSystem)
	{
		ActiveEffectComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			TravelEffectSystem,
			RootComponent,
			NAME_None,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget,
			true
		);
	}

	SetActorTickEnabled(true);
}

void AMagicHandProjectile::EnterTetherAnchorMode(AActor* TetherTarget)
{
	bInAnchorMode = true;

	ProjectileMovement->StopMovementImmediately();
	ProjectileMovement->Deactivate();

	// No longer detecting possessables.
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (IsValid(ActiveEffectComponent))
	{
		ActiveEffectComponent->DeactivateImmediate();
	}

	if (AnchorEffectSystem)
	{
		ActiveEffectComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			AnchorEffectSystem,
			RootComponent,
			NAME_None,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget,
			true
		);
	}

	if (TetherTarget)
	{
		AttachToActor(TetherTarget, FAttachmentTransformRules::KeepWorldTransform);
	}
}

void AMagicHandProjectile::OnSphereBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult
)
{
	if (bInAnchorMode || !OtherActor)
	{
		return;
	}

	// Skip the owning character.
	if (OtherActor == GetInstigator() || OtherActor == GetOwner())
	{
		return;
	}

	if (!OtherActor->Implements<UMagicHandPossessable>())
	{
		return;
	}

	if (OwnerMagicHandComponent.IsValid())
	{
		OwnerMagicHandComponent->NotifyProjectileHitPossessable(OtherActor, this);
	}
}
