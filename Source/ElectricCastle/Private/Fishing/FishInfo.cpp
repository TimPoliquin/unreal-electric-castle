// Copyright Alien Shores


#include "Fishing/FishInfo.h"

FFishDefinition UFishInfo::GetFishDefinitionByFishType(const FGameplayTag& FishType) const
{
	const FFishDefinition* Match = FishInfos.FindByPredicate(
		[FishType](const FFishDefinition& FishInfo)
		{
			return FishInfo.FishType == FishType;
		}
	);
	if (Match)
	{
		return *Match;
	}
	return FFishDefinition();
}

float UFishInfo::GetFishRarityMultiplierByRarity(const FGameplayTag& Rarity) const
{
	const FFishRarity* Match = FishRarityInfos.FindByPredicate(
		[Rarity](const FFishRarity& FishRarity)
		{
			return FishRarity.Rarity == Rarity;
		}
	);
	return Match
		       ? Match->RarityMultiplier
		       : 0.f;
}

float UFishInfo::GetFishRarityMultiplierByFishType(const FGameplayTag& FishType) const
{
	return GetFishRarityMultiplierByRarity(GetFishDefinitionByFishType(FishType).Rarity);
}

float UFishInfo::GetFishRarityMultiplierByPlayerFishingLevel(
	const int32& PlayerFishingLevel,
	const FGameplayTag& Rarity
) const
{
	const FPlayerFishingLevelRarity* PlayerLevelRarity = PlayerFishingLevelRarityInfos.FindByPredicate(
		[PlayerFishingLevel, Rarity](const FPlayerFishingLevelRarity& PlayerLevelRarityInfo)
		{
			return PlayerLevelRarityInfo.Level == PlayerFishingLevel && PlayerLevelRarityInfo.RarityMultipliers.
			                                                                                  Contains(Rarity);
		}
	);
	if (PlayerLevelRarity)
	{
		return PlayerLevelRarity->RarityMultipliers[Rarity];
	}
	return 1.f;
}
