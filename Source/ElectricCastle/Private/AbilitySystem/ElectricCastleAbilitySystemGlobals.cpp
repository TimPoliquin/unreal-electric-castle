// Copyright Alien Shores


#include "AbilitySystem/ElectricCastleAbilitySystemGlobals.h"

#include "AbilitySystem/ElectricCastleAbilitySystemTypes.h"

FGameplayEffectContext* UElectricCastleAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FElectricCastleGameplayEffectContext();
}
