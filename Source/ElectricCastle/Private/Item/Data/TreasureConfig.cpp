// Copyright Alien Shores


#include "Item/Data/TreasureConfig.h"

#include "Utils/ArrayUtils.h"

FTreasureConfigRow UTreasureConfig::GetTreasureConfig(const float Value, const bool Randomize) const
{
	FTreasureConfigRow Config = FTreasureConfigRow();
	const FTreasureConfigOption* Option = TreasureConfigs.FindByPredicate([Value](const FTreasureConfigOption& Current)
	{
		return Value >= Current.Range.Min && Value < Current.Range.Max;
	});
	if (Option)
	{
		Config.Mesh = UArrayUtils::GetRandomElement(Option->Meshes).LoadSynchronous();
		Config.Value = FMath::RoundHalfToEven(Randomize ? Option->Range.Value() : Value);
	}
	return Config;
}
