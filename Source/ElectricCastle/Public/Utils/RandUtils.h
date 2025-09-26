#pragma once

#include "CoreMinimal.h"
#include "RandUtils.generated.h"

USTRUCT(BlueprintType)
struct FRandRange
{
	GENERATED_BODY()

	FRandRange()
	{
		Min = 0.f;
		Max = 0.f;
	}

	FRandRange(const float InMin, const float InMax)
	{
		Min = InMin;
		Max = InMax;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Min = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Max = 0.f;

	float Value() const
	{
		return FMath::FRandRange(Min, Max);
	}
};

UCLASS()
class ELECTRICCASTLE_API URandRangeBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static float RandRange(const FRandRange& Range)
	{
		return Range.Value();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool ShouldAct(const float Chance)
	{
		return FMath::FRandRange(0.f, 1.f) <= Chance;
	}
};
