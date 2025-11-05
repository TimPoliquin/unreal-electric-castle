// Copyright Alien Shores


#include "AbilitySystem/Ability/Offensive/FireBlastGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "Actor/Projectile/FireballProjectile.h"

FString UFireBlastGameplayAbility::GetDescription_Implementation(const int32 AbilityLevel) const
{
	const float ManaCost = GetManaCost(AbilityLevel);
	const float Cooldown = GetCooldown(AbilityLevel);
	const int32 Damage = GetDamageAtLevel(AbilityLevel);
	const FString Bolts = AbilityLevel == 1
		                      ? FString::Printf(TEXT("a bolt"))
		                      : FString::Printf(
			                      TEXT("%d bolts"),
			                      NumFireballs
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
			RICH_DEFAULT("Launches %s fireballs in all directions, each coming back and exploding upon return, causing "
			) RICH_DAMAGE("%d")
			RICH_DEFAULT(
				" radial fire damage with a chance to burn."
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

TArray<AFireballProjectile*> UFireBlastGameplayAbility::SpawnFireballs()
{
	TArray<AFireballProjectile*> Fireballs;
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	const TArray<FRotator> Rotators = UElectricCastleAbilitySystemLibrary::EvenlySpacedRotators(
		Forward,
		FVector::UpVector,
		360.f,
		NumFireballs
	);
	for (const FRotator& Rotator : Rotators)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(Location);
		SpawnTransform.SetRotation(Rotator.Quaternion());
		AFireballProjectile* Fireball = GetWorld()->SpawnActorDeferred<AFireballProjectile>(
			FireballClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			CurrentActorInfo->PlayerController->GetPawn(),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		);
		Fireball->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults(nullptr);
		Fireball->ReturnToActor = GetAvatarActorFromActorInfo();
		Fireball->SetupExplosionConfig(DamageEffectClass, ExplosionDamageConfig);
		Fireball->SetOwner(GetAvatarActorFromActorInfo());
		Fireballs.Add(Fireball);
		Fireball->FinishSpawning(SpawnTransform);
	}
	return Fireballs;
}
