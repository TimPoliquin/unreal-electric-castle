// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/ElectricCastleGameplayAbility.h"
#include "Fishing/FishTypes.h"
#include "FishingReelGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UFishingReelGameplayAbility : public UElectricCastleGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void ReelIn(AActor* Player);

private:
	UFUNCTION()
	void OnFishingStateChanged(EFishingState FishingState);
};
