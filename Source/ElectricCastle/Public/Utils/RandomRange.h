// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RandomRange.generated.h"

/**
 * 
 */

UCLASS(BlueprintType, Blueprintable, DisplayName="Random Range")
class ELECTRICCASTLE_API URandomRange : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetValue() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetMinValue() const
	{
		return Range.GetLowerBound().IsClosed() ? Range.GetLowerBoundValue() : FLT_MIN;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetMaxValue() const
	{
		return Range.GetUpperBound().IsClosed() ? Range.GetUpperBoundValue() : FLT_MAX;
	}

	void SetRange(const FFloatRange InRange);
	void SetRange(const float InMin, const float InMax);

protected:
	FFloatRange Range;
};
