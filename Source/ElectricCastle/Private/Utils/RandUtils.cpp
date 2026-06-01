#include "Utils/RandUtils.h"

#include "Kismet/KismetMathLibrary.h"

float URandRangeBlueprintLibrary::GetRandomFloatInRange(const FFloatRange Range)
{
	const float Min = Range.GetLowerBound().IsClosed() ? Range.GetLowerBoundValue() : FLT_MIN;
	const float Max = Range.GetUpperBound().IsClosed() ? Range.GetUpperBoundValue() : FLT_MAX;
	return UKismetMathLibrary::RandomFloatInRange(Min, Max);
}

float URandRangeBlueprintLibrary::GetRandomInt32InRange(const FInt32Range Range)
{
	const int32 Min = Range.GetLowerBound().IsClosed() ? Range.GetLowerBoundValue() : INT32_MIN;
	const int32 Max = Range.GetUpperBound().IsClosed() ? Range.GetUpperBoundValue() : INT32_MAX;
	return UKismetMathLibrary::RandomFloatInRange(Min, Max);
}
