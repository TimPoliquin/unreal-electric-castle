// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/Passive/PassiveGameplayAbility.h"
#include "HaloOfProtectionGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UHaloOfProtectionGameplayAbility : public UPassiveGameplayAbility
{
	GENERATED_BODY()

public:
	virtual FString GetDescription_Implementation(const int32 AbilityLevel) const override;
	float GetProtectionAtLevel(int32 Level) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FScalableFloat ProtectionConfig;
};
