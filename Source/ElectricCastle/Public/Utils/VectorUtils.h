// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VectorUtils.generated.h"

UENUM(BlueprintType)
enum class EVectorDirection : uint8
{
	None,
	Forward,
	Backward,
	Right,
	Left,
	Count
};

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UVectorUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static EVectorDirection GetInputVectorDirection(AActor* Actor, const FVector& InputDirection);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FVector ComputeOrbitTarget(const FVector& StartingLocation, const FVector& Center, const FVector& DodgeDirection, float Distance);
};
