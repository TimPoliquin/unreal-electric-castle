// Copyright Alien Shores


#include "Actor/Projectile/ArrowProjectile.h"

#include "NiagaraComponent.h"
#include "Components/CapsuleComponent.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/LockOn/LockOnActor.h"
#include "Player/LockOn/LockOnController.h"


// Sets default values
AArrowProjectile::AArrowProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	TrailFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Trail FX"));
	TrailFX->SetupAttachment(CollisionComponent);
	TrailFX->SetAutoActivate(false);
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	MeshComponent->SetupAttachment(CollisionComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ProjectileMovement->SetAutoActivate(false);
}


// Called every frame
void AArrowProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// update the projectile vertical angle to match the velocity
	if (ProjectileMovement->IsActive())
	{
		if (const FVector Vel = GetVelocity(); !Vel.IsNearlyZero())
		{
			SetActorRotation(FRotationMatrix::MakeFromX(Vel).Rotator());
		}
	}
}

void AArrowProjectile::OnPool_Returned_Implementation(const FSpawnPoolEventPayload& Payload)
{
	Super::OnPool_Returned_Implementation(Payload);
	TrailFX->Deactivate();
	MeshComponent->Deactivate();
	MeshComponent->SetVisibility(false);
	ProjectileMovement->ProjectileGravityScale = 0.f;
	ProjectileMovement->Deactivate();
}

void AArrowProjectile::OnPool_FinishRetrieve_Implementation(const FSpawnPoolEventPayload& Payload)
{
	Super::OnPool_FinishRetrieve_Implementation(Payload);
	MeshComponent->Activate();
	MeshComponent->SetVisibility(true);
	ProjectileMovement->StopMovementImmediately();
	ProjectileMovement->ProjectileGravityScale = 0.f;
	ProjectileMovement->Deactivate();
}

void AArrowProjectile::Release_Implementation()
{
	FVector TargetLocation;
	const AActor* TargetActor = nullptr;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(GetInstigator());
	if (const ULockOnController* AimController = ILockOnActor::GetLockOnController(GetInstigator()))
	{
		TargetActor = AimController->GetLockOnTarget();
	}
	if (IsValid(TargetActor))
	{
		TargetLocation = TargetActor->GetActorLocation();
	}
	else
	{
		FHitResult HitResult;
		FCollisionQueryParams Params;
		Params.AddIgnoredActors(ActorsToIgnore);
		GetWorld()->LineTraceSingleByChannel(HitResult, GetActorLocation(), GetActorLocation() + GetActorForwardVector() * ProjectileMovement->InitialSpeed, ECC_Visibility, Params);
		if (HitResult.bBlockingHit)
		{
			TargetLocation = HitResult.ImpactPoint;
		}
		else
		{
			TargetLocation = GetActorLocation() + GetActorForwardVector() * ProjectileMovement->InitialSpeed;
		}
	}
	if (bDebug)
	{
		DrawDebugSphere(GetWorld(), TargetLocation, 20, 12, FColor::Red, false, 1, 0, 1);
	}
	FVector NewVelocity;
	UGameplayStatics::FSuggestProjectileVelocityParameters ProjectileParams = UGameplayStatics::FSuggestProjectileVelocityParameters(
		GetWorld(),
		GetActorLocation(),
		TargetLocation,
		ProjectileMovement->InitialSpeed
	);
	ProjectileParams.bFavorHighArc = false;
	ProjectileParams.CollisionRadius = CollisionComponent->GetScaledCapsuleRadius();
	ProjectileParams.OverrideGravityZ = 0;
	ProjectileParams.TraceOption = ESuggestProjVelocityTraceOption::OnlyTraceWhileAscending;
	ProjectileParams.ResponseParam = FCollisionResponseParams::DefaultResponseParam;
	ProjectileParams.ActorsToIgnore = ActorsToIgnore;
	ProjectileParams.bDrawDebug = bDebug;
	ProjectileParams.bAcceptClosestOnNoSolutions = true;
	ProjectileParams.ActorsToIgnore = ActorsToIgnore;
	if (!UGameplayStatics::SuggestProjectileVelocity(ProjectileParams, NewVelocity))
	{
		if (bDebug)
		{
			UE_LOG(LogElectricCastle, Warning, TEXT("[%s] No valid velocity found - creating one based on forward vector"), *GetName())
		}
		NewVelocity = ProjectileMovement->InitialSpeed * GetActorForwardVector();
	}
	ProjectileMovement->Velocity = NewVelocity;
	if (bDebug)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s] fire velocity: %s"), *GetName(), *NewVelocity.ToString());
	}
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 1.f;
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TrailFX->Activate();
	ProjectileMovement->Activate();
}
