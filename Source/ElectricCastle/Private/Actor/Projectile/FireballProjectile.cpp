// Copyright Alien Shores


#include "Actor/Projectile/FireballProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "Net/UnrealNetwork.h"


AFireballProjectile::AFireballProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AFireballProjectile::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFireballProjectile, ExplosionDamageConfig);
}

bool AFireballProjectile::IsWithinExplodeDistance() const
{
	return FVector::Distance(
		GetActorLocation(),
		GetReturnToLocation()
	) <= ExplodeDistance;
}

FVector AFireballProjectile::GetReturnToLocation() const
{
	return IsValid(ReturnToActor)
		       ? ReturnToActor->GetActorLocation()
		       : InitialLocation;
}

void AFireballProjectile::Explode_Implementation()
{
	TArray<AActor*> TargetActors = FindImpactTargets();
	for (AActor* TargetActor : TargetActors)
	{
		ExplodeOnTarget(TargetActor);
	}
}

void AFireballProjectile::ExplodeOnTarget(
	AActor* Target
)
{
	if (!IsValid(Target))
	{
		return;
	}
	FDamageEffectParams ExplosionDamageEffectParams = UElectricCastleAbilitySystemLibrary::MakeCustomDamageEffectParams(
		GetOwner(),
		Target,
		ExplosionDamageEffectClass,
		ExplosionDamageConfig,
		AbilityLevel,
		GetActorLocation()
	);
	UElectricCastleAbilitySystemLibrary::ApplyDamageEffect(
		ExplosionDamageEffectParams
	);
}

void AFireballProjectile::SetupExplosionConfig(
	const TSubclassOf<UGameplayEffect>& InExplosionDamageEffectClass,
	const FElectricCastleDamageConfig& InExplosionDamageConfig,
	const int32 InAbilityLevel
)
{
	ExplosionDamageEffectClass = InExplosionDamageEffectClass;
	ExplosionDamageConfig = InExplosionDamageConfig;
	AbilityLevel = InAbilityLevel;
}

void AFireballProjectile::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		InitialLocation = GetActorLocation();
		ApexLocation = GetActorForwardVector() * TravelDistance + InitialLocation;
		StartOutgoingTimeline();
	}
}

void AFireballProjectile::OnSphereOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (!IsValidOverlap(OtherActor))
	{
		return;
	}
	if (HasAuthority())
	{
		// DEVNOTE - this was hack copied out of AuraProjectile. This probably should have been refactored to be configurable instead.
		if (UAbilitySystemComponent* OtherAbilitySystem = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
			OtherActor
		))
		{
			DamageEffectParams.DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
			DamageEffectParams.TargetAbilitySystemComponent = OtherAbilitySystem;
			UElectricCastleAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		}
	}
}
