// Copyright Alien Shores


#include "Actor/Horde/HordeModeConfig.h"

FHordeRoundConfig UHordeModeConfig::GetRoundConfig(const int32 RoundNum) const
{
	if (RoundConfigs.IsValidIndex(RoundNum))
	{
		return RoundConfigs[RoundNum];
	}
	return FHordeRoundConfig();
}

bool UHordeModeConfig::HasConfigurationForRound(const int32 RoundNum) const
{
	return RoundConfigs.IsValidIndex(RoundNum);
}

TArray<FHordeRoundRewards> UHordeModeConfig::GetRewards(const int32 RoundNum) const
{
	return GetRoundConfig(RoundNum).Rewards;
}

int32 UHordeModeConfig::GetNumberOfRewardsToSpawn(TArray<FHordeRoundRewards> Rewards)
{
	int32 NumberOfRewardsToSpawn = 0;
	for (const auto& [Reward, Count] : Rewards)
	{
		NumberOfRewardsToSpawn += Count;
	}
	return NumberOfRewardsToSpawn;
}
