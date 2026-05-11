#pragma once

#include "CoreMinimal.h"
#include "RandUtils.generated.h"

USTRUCT(BlueprintType)
struct FRandRange
{
	GENERATED_BODY()

	UE_DEPRECATED(5.7, "FRandRange is deprecated. Use FFloatRange instead!")
	FRandRange()
	{
	}

	UE_DEPRECATED(5.7, "FRandRange is deprecated. Use FFloatRange instead!")
	FRandRange(const float InRange) : Min(InRange), Max(InRange)
	{
	}

	UE_DEPRECATED(5.7, "FRandRange is deprecated. Use FFloatRange instead!")
	FRandRange(const float InMin, const float InMax) : Min(InMin), Max(InMax)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Min = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Max = 0.f;

	UE_DEPRECATED(5.7, "FRandRange is deprecated. Use FFloatRange instead!")
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
	static bool ShouldAct(const float Chance)
	{
		return FMath::FRandRange(0.f, 1.f) <= Chance;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static float GetRandomFloatInRange(const FFloatRange Range);
};
