// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "FishTypes.h"
#include "Engine/DataAsset.h"
#include "FishInfo.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UFishInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	FFishDefinition GetFishDefinitionByFishType(const FGameplayTag& FishType) const;
	UFUNCTION(BlueprintCallable)
	float GetFishRarityMultiplierByRarity(const FGameplayTag& Rarity) const;
	UFUNCTION(BlueprintCallable)
	float GetFishRarityMultiplierByFishType(const FGameplayTag& FishType) const;
	UFUNCTION(BlueprintCallable)
	float GetFishRarityMultiplierByPlayerFishingLevel(
		const int32& PlayerFishingLevel,
		const FGameplayTag& Rarity
	) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FishInfo")
	TArray<FFishDefinition> FishInfos;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FishInfo")
	TArray<FFishRarity> FishRarityInfos;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FishInfo")
	TArray<FPlayerFishingLevelRarity> PlayerFishingLevelRarityInfos;
};
