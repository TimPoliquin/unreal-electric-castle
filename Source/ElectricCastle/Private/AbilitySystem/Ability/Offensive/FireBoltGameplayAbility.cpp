// Copyright Alien Shores


#include "AbilitySystem/Ability/Offensive/FireBoltGameplayAbility.h"

#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "ElectricCastle/ElectricCastle.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

FString UFireBoltGameplayAbility::GetDescription_Implementation(const int32 AbilityLevel) const
{
	const float ManaCost = GetManaCost(AbilityLevel);
	const float Cooldown = GetCooldown(AbilityLevel);
	const int32 Damage = GetDamageAtLevel(AbilityLevel);
	const FString Bolts = AbilityLevel == 1
		                      ? FString::Printf(TEXT("a bolt"))
		                      : FString::Printf(
			                      TEXT("%d bolts"),
			                      GetNumProjectiles(AbilityLevel)
		                      );
	return FString::Printf(
		TEXT(
			// Title
			"" RICH_TITLE("%s") "\n\n"
			// Details
			RICH_SMALL("Level: ") RICH_LEVEL("%d") "\n" // Level
			RICH_SMALL("Mana Cost: ") RICH_MANA_COST("%.1f") "\n" // Mana Cost
			RICH_SMALL("Cooldown: ") RICH_COOLDOWN("%.1f") "\n\n" // Cooldown
			// Description
			RICH_DEFAULT("Launches %s of fire, exploding on impact and dealing ") RICH_DAMAGE("%d")
			RICH_DEFAULT(
				"fire damage with a chance to burn."
			) "\n\n"
		),
		*GetAbilityName(),
		AbilityLevel,
		FMath::Abs(ManaCost),
		FMath::Abs(Cooldown),
		*Bolts,
		Damage
	);
}

void UFireBoltGameplayAbility::SpawnProjectiles(
	const FVector& ProjectileTargetLocation,
	const FGameplayTag& SocketTag,
	AActor* Target
)
{
	if (!GetAvatarActorFromActorInfo()->HasAuthority())
	{
		return;
	}
	const FVector SpawnLocation = GetProjectileSpawnLocation(SocketTag);
	const FRotator Rotation = GetProjectileSpawnRotation(ProjectileTargetLocation, SpawnLocation, Target);
	const FVector Forward = Rotation.Vector();
	const TArray<FRotator> Rotations = UElectricCastleAbilitySystemLibrary::EvenlySpacedRotators(
		Forward,
		FVector::UpVector,
		ProjectileSpread,
		GetNumProjectiles(GetAbilityLevel())
	);
	FOnSpawnProjectileFinishedSignature OnSpawnFinish;
	OnSpawnFinish.BindLambda(
		[this, Target](AProjectileActor* SpawnedProjectile)
		{
			SpawnedProjectile->SetHomingTarget(Target);
			SpawnedProjectile->GetProjectileMovementComponent()->HomingAccelerationMagnitude = FMath::FRandRange(
				HomingAccelerationMin,
				HomingAccelerationMax
			);
		}
	);
	for (const FRotator& ProjectileRotation : Rotations)
	{
		SpawnProjectile(
			SpawnLocation,
			ProjectileRotation,
			&OnSpawnFinish
		);
	}
}

int32 UFireBoltGameplayAbility::GetNumProjectiles(const int32 AbilityLevel) const
{
	return FMath::RoundToInt32(ProjectileCount.GetValueAtLevel(AbilityLevel));
}

AActor* UFireBoltGameplayAbility::FindTargetAlongProjectilePath(const FVector& SpawnLocation, const FVector& Forward) const
{
	FHitResult CursorHit;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
	UKismetSystemLibrary::SphereTraceSingle(
		GetAvatarActorFromActorInfo(),
		SpawnLocation,
		SpawnLocation + Forward * TraceRange,
		50,
		UEngineTypes::ConvertToTraceType(ECC_Target),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		CursorHit,
		true
	);
	return CursorHit.GetActor();
}
