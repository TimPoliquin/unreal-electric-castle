// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AbilityRangeConfig.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class ELECTRICCASTLE_API UAbilityRangeConfig : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	bool IsInSupportedRange(const float Distance) const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FFloatRange GetPreferredDistanceRange() const;
	UFUNCTION(BlueprintCallable)
	float GetPreferredDistance() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FFloatRange SupportedRange = FFloatRange::Inclusive(0.f, 200.f);
};
