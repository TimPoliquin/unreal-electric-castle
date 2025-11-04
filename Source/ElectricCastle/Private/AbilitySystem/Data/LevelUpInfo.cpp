// Copyright Alien Shores


#include "AbilitySystem/Data/LevelUpInfo.h"

FLevelUpInfoRow ULevelUpInfo::FindLevelUpInfoByXP(const int32 XP) const
{
	const FLevelUpInfoRow* Match = LevelUpInfos.FindByPredicate(
		[XP](const FLevelUpInfoRow& LevelUpInfo)
		{
			return XP < LevelUpInfo.LevelUpRequirement;
		}
	);
	if (Match != nullptr)
	{
		return *Match;
	}
	return LevelUpInfos[0];
}

int32 ULevelUpInfo::FindLevelByXP(const int32 XP) const
{
	for (int32 i = 0; i < LevelUpInfos.Num(); i++)
	{
		if (XP < LevelUpInfos[i].LevelUpRequirement)
		{
			return i;
		}
	}
	return 0;
}

FLevelUpRewards ULevelUpInfo::GetRewardsByLevel(const int32 InLevel) const
{
	return LevelUpInfos[InLevel].GetRewards();
}

float ULevelUpInfo::GetLevelProgressPercentage(const int32 CurrentXP) const
{
	const int CurrentLevel = FindLevelByXP(CurrentXP);
	const int LevelEndXP = LevelUpInfos[CurrentLevel].LevelUpRequirement;
	int LevelStartXP = 0;
	if (CurrentLevel > 0)
	{
		LevelStartXP = LevelUpInfos[CurrentLevel - 1].LevelUpRequirement;
	}
	if (LevelEndXP > LevelStartXP && LevelEndXP > 0)
	{
		const float Percentage = static_cast<float>(CurrentXP - LevelStartXP) / static_cast<float>(LevelEndXP -
			LevelStartXP);
		return Percentage;
	}
	return 0;
}
