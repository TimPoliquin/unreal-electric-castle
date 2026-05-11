// Copyright Alien Shores


#include "Utils/RandomRange.h"

#include "Utils/RandUtils.h"

float URandomRange::GetValue() const
{
	return URandRangeBlueprintLibrary::GetRandomFloatInRange(Range);
}

void URandomRange::SetRange(const FFloatRange InRange)
{
	Range = InRange;
}

void URandomRange::SetRange(const float InMin, const float InMax)
{
	Range = FFloatRange::Inclusive(InMin, InMax);
}
