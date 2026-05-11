// Copyright Alien Shores


#include "AbilitySystem/Data/Config/AbilityEngagementConfig.h"

#include "Utils/ArrayUtils.h"

bool UAbilityEngagementConfig::IsAbilitySupportedInRange(const EEngagementRange InEngagementRange) const
{
	return SupportedEngagementRanges.Contains(InEngagementRange);
}

bool UAbilityEngagementConfig::IsAbilitySupportedInModes(const TArray<EEngagementAbilityMode>& InEngagementAbilityModes) const
{
	return UArrayUtils::ContainsAny(SupportedEngagementAbilityModes, InEngagementAbilityModes);
}
