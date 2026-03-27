// Copyright Alien Shores


#include "AbilitySystem/Ability/Offensive/ProjectileGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actor/Projectile/ProjectileActor.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Interaction/CombatInterface.h"
#include "Templates/Function.h"
#include "World/Pool/WorldPooledActorSubsystem.h"

void UProjectileGameplayAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

AProjectileActor* UProjectileGameplayAbility::SpawnProjectile(
	const FVector& ProjectileTargetLocation,
	const AActor* HitActor,
	const FGameplayTag& SocketTag
)
{
	if (const AActor* OwningActor = GetAvatarActorFromActorInfo(); !OwningActor->HasAuthority())
	{
		// Do not execute on client - run on server only
		return nullptr;
	}
	check(ProjectileClass);
	const FVector SpawnLocation = GetProjectileSpawnLocation(SocketTag);
	const FRotator Rotation = GetProjectileSpawnRotation(ProjectileTargetLocation, SpawnLocation, HitActor);
	return SpawnProjectile(SpawnLocation, Rotation);
}

void UProjectileGameplayAbility::FireProjectileAtTarget_Implementation(const FGameplayTag& MontageTag)
{
	const FVector& TargetLocation = GetFaceTargetLocation();
	SpawnProjectile(TargetLocation, nullptr, MontageTag);
}

FVector UProjectileGameplayAbility::GetProjectileSpawnLocation(const FGameplayTag& MontageTag) const
{
	return ICombatInterface::Execute_GetCombatSocketLocation(
		GetAvatarActorFromActorInfo(),
		MontageTag
	);
}

FRotator UProjectileGameplayAbility::GetProjectileSpawnRotation(
	const FVector& TargetLocation,
	const FVector& SpawnLocation,
	const AActor* TargetActor
) const
{
	FRotator Rotation = (TargetLocation - SpawnLocation).Rotation();
	if (!ICombatInterface::IsAlive(TargetActor))
	{
		// reset pitch if the hit actor is not something that can be targeted
		Rotation.Pitch = Pitch;
	}
	return Rotation;
}

AProjectileActor* UProjectileGameplayAbility::SpawnProjectile(
	const FVector& SpawnLocation,
	const FRotator& SpawnRotation,
	const FOnSpawnProjectileFinishedSignature* BeforeFinishSpawning
)
{
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SpawnLocation);
	SpawnTransform.SetRotation(SpawnRotation.Quaternion());

	AProjectileActor* SpawnedProjectile = nullptr;
	if (UWorldPooledActorSubsystem* Pool = UWorldPooledActorSubsystem::Get(GetAvatarActorFromActorInfo()))
	{
		SpawnedProjectile = Cast<AProjectileActor>(ISpawnPoolInterface::AcquireDeferred(Pool, GetAvatarActorFromActorInfo(), ProjectileClass));
		ConfigureProjectile(SpawnedProjectile);
		if (BeforeFinishSpawning)
		{
			BeforeFinishSpawning->ExecuteIfBound(SpawnedProjectile);
		}
		ISpawnPoolInterface::AcquireFinalize(Pool, SpawnedProjectile, SpawnTransform);
	}
	else
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Failed to get projectile from spawn pool!"), *GetName());
	}

	return SpawnedProjectile;
}

AProjectileActor* UProjectileGameplayAbility::SpawnProjectile_Basic()
{
	if (const AActor* OwningActor = GetAvatarActorFromActorInfo(); !OwningActor->HasAuthority())
	{
		// Do not execute on client - run on server only
		return nullptr;
	}
	check(ProjectileClass);
	return SpawnProjectile(
		GetAvatarActorFromActorInfo()->GetActorLocation(),
		GetAvatarActorFromActorInfo()->GetActorForwardVector().Rotation()
	);
}

void UProjectileGameplayAbility::ConfigureProjectile_Implementation(AProjectileActor* Projectile) const
{
	Projectile->SetInstigator(Cast<APawn>(GetAvatarActorFromActorInfo()));
	Projectile->SetOwner(GetAvatarActorFromActorInfo());
	ApplyDefaultDamageConfig(Projectile);
}

FGameplayEffectSpecHandle UProjectileGameplayAbility::MakeDamageEffectSpecHandle(
	AActor* SourceObject,
	const FVector& TargetLocation
) const
{
	const UAbilitySystemComponent* SourceAbilitySystem = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
		GetAvatarActorFromActorInfo()
	);
	FGameplayEffectContextHandle ContextHandle = SourceAbilitySystem->MakeEffectContext();
	ContextHandle.SetAbility(this);
	ContextHandle.AddSourceObject(SourceObject);
	// Add actors to the context
	TArray<TWeakObjectPtr<AActor>> Actors;
	Actors.Add(SourceObject);
	ContextHandle.AddActors(Actors);
	// Set hit result on the context
	FHitResult HitResult;
	HitResult.Location = TargetLocation;
	ContextHandle.AddHitResult(HitResult);
	// Create outgoing spec
	return SourceAbilitySystem->MakeOutgoingSpec(
		DamageEffectClass,
		GetAbilityLevel(),
		ContextHandle
	);
}
