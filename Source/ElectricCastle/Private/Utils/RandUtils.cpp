#include "Utils/RandUtils.h"

#include "Kismet/KismetMathLibrary.h"

float URandRangeBlueprintLibrary::GetRandomFloatInRange(const FFloatRange Range)
{
	const float Min = Range.GetLowerBound().IsClosed() ? Range.GetLowerBoundValue() : FLT_MIN;
	const float Max = Range.GetUpperBound().IsClosed() ? Range.GetUpperBoundValue() : FLT_MAX;
	return UKismetMathLibrary::RandomFloatInRange(Min, Max);
}
