// Copyright Alien Shores


#include "AbilitySystem/Data/Config/AbilityRangeConfig.h"

#include "Utils/RandUtils.h"

bool UAbilityRangeConfig::IsInSupportedRange(const float Distance) const
{
	return SupportedRange.Contains(Distance);
}

FFloatRange UAbilityRangeConfig::GetPreferredDistanceRange() const
{
	return SupportedRange;
}

float UAbilityRangeConfig::GetPreferredDistance() const
{
	return URandRangeBlueprintLibrary::GetRandomFloatInRange(SupportedRange);
}
