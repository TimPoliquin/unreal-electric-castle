// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "ElectricCastleAbilitySystemGlobals.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UElectricCastleAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()

public:
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
};
