// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TrackFunctionLibrary.generated.h"

class USplineComponent;
/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UTrackFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ElectricCastle|Track")
	static FVector GetLocationOnTrack(const USplineComponent* Spline, const float Progress);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ElectricCastle|Track")
	static FRotator GetRotationOnTrack(const USplineComponent* Spline, const float Progress);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ElectricCastle|Track")
	static bool HasMoreTrack(const USplineComponent* Spline, const float Progress);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ElectricCastle|Track")
	static float GetClosestDistanceOnTrack(const USplineComponent* Spline, const FVector& Location);
};
