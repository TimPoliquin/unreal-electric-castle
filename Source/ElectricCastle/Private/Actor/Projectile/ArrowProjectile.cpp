// Copyright Alien Shores


#include "Actor/Projectile/ArrowProjectile.h"

#include "NiagaraComponent.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Net/UnrealNetwork.h"


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
	if (bMatchOwnerForward && GetOwner())
	{
		const FRotator OwnerRotation = GetOwner()->GetActorRotation();
		const FRotator NewRotation(Pitch, OwnerRotation.Yaw, OwnerRotation.Roll);
		SetActorRotation(NewRotation);
	}
}

void AArrowProjectile::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AArrowProjectile, bMatchOwnerForward)
	DOREPLIFETIME(AArrowProjectile, Pitch)
}

void AArrowProjectile::Release_Implementation()
{
	const AActor* HitTarget = UElectricCastleAbilitySystemLibrary::FindHitBySphereTrace(bMatchOwnerForward ? GetOwner() : this, CollisionComponent->GetScaledCapsuleRadius());
	float ReleasePitch = Pitch;
	if (IsValid(HitTarget))
	{
		UElectricCastleAbilitySystemLibrary::CalculatePitchToHitTarget(GetActorLocation(), HitTarget->GetActorLocation(), ProjectileMovement->InitialSpeed, ReleasePitch);
	}
	const FVector ForwardVector = bMatchOwnerForward ? GetOwner()->GetActorForwardVector() : GetActorForwardVector();
	// Get the actor's right vector (axis to rotate around)
	const FVector RightVector = bMatchOwnerForward ? GetOwner()->GetActorRightVector() : GetActorRightVector();
	// Create a rotation of +10 degrees around the right axis
	const FQuat RotationQuat = FQuat(RightVector, FMath::DegreesToRadians(-1 * ReleasePitch));
	// Apply the rotation to the forward vector
	const FVector NewForward = RotationQuat.RotateVector(ForwardVector);
	ProjectileMovement->Velocity = NewForward * ProjectileMovement->InitialSpeed;
	SetMatchOwnerForward(false);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TrailFX->Activate();
	ProjectileMovement->Activate();
	SetLifeSpan(LifeSpan);
}

void AArrowProjectile::SetMatchOwnerForward_Implementation(const bool bInMatchOwnerPitch)
{
	bMatchOwnerForward = bInMatchOwnerPitch;
}
