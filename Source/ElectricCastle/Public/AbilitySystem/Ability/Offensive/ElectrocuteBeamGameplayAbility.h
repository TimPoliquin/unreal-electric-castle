// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "BeamGameplayAbility.h"
#include "ElectrocuteBeamGameplayAbility.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API UElectrocuteBeamGameplayAbility : public UBeamGameplayAbility
{
	GENERATED_BODY()

public:
	virtual FString GetDescription_Implementation(const int32 AbilityLevel) const override;
};
