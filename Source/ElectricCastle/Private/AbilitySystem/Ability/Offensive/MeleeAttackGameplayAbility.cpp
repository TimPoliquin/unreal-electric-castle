// Copyright Alien Shores


#include "AbilitySystem/Ability/Offensive/MeleeAttackGameplayAbility.h"

#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"

void UMeleeAttackGameplayAbility::DamageTargets_Implementation(const TArray<AActor*>& Targets, const FVector& ImpactLocation, const FGameplayTag& MontageTag)
{
	for (AActor* Target : Targets)
	{
		const FDamageEffectParams DamageEffectParams = MakeDamageEffectParamsFromClassDefaults(Target, ImpactLocation);
		UElectricCastleAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		if (ImpactCueTag.IsValid())
		{
			const FGameplayCueParameters ImpactCueParams = MakeGameplayCueParamsFromMontageTag(MontageTag, ImpactLocation);
			K2_ExecuteGameplayCueWithParams(ImpactCueTag, ImpactCueParams);
		}
	}
}
