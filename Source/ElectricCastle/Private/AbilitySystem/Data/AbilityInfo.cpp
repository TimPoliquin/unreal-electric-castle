// Copyright Alien Shores


#include "AbilitySystem/Data/AbilityInfo.h"

#include "ElectricCastle/ElectricCastleLogChannels.h"

FElectricCastleAbilityInfo UAbilityInfo::FindAbilityInfoForTag(const FGameplayTag AbilityTag, bool bLogNotFound) const
{
	for (const FElectricCastleAbilityInfo& AbilityInfo : AbilityInformation)
	{
		if (AbilityInfo.AbilityTag.MatchesTagExact(AbilityTag))
		{
			return AbilityInfo;
		}
	}
	if (bLogNotFound)
	{
		UE_LOG(
			LogElectricCastle,
			Error,
			TEXT("Unable to find ability by tag [%s] on Ability Info [%s]"),
			*AbilityTag.ToString(),
			*GetNameSafe(this)
		);
	}
	return FElectricCastleAbilityInfo();
}
