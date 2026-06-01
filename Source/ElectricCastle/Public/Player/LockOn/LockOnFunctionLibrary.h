// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LockOnFunctionLibrary.generated.h"

USTRUCT()
struct FLockOnCandidateScreenSpaceData
{
	GENERATED_BODY()
	UPROPERTY()
	bool bIsOnScreen = false;
	UPROPERTY()
	FVector2D ScreenPosition = FVector2D::ZeroVector;
	UPROPERTY()
	float ScreenScore = 0.f;
};

USTRUCT()
struct FLockOnCandidateEntry
{
	GENERATED_BODY()
	UPROPERTY()
	TObjectPtr<AActor> Target;
	UPROPERTY()
	float Distance = FLT_MAX;
	UPROPERTY()
	float DistanceScore = 0.f;
	UPROPERTY()
	float TotalScore = 0.f;
	UPROPERTY()
	FLockOnCandidateScreenSpaceData ScreenSpaceData;
};

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API ULockOnFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static AActor* FindClosestTarget(
		const APlayerController* PlayerController, const float LockOnRadius, const bool bRequireLineOfSight = true, const bool bDebug = false
	);
	UFUNCTION(BlueprintCallable)
	static AActor* SwitchTarget(
		const APlayerController* PlayerController,
		const AActor* CurrentTarget,
		const FVector2D& StickInput,
		float LockOnRadius, bool bRequireLineOfSight, bool bDebug = true
	);
	static bool IsTargetValid(const AActor* Target);

protected:
	static TArray<FLockOnCandidateEntry> GatherCandidates(const APlayerController* PlayerController, float TargetingRadius, bool bScore, const TArray<const AActor*>& IgnoreActors, bool bDebug);
	static FLockOnCandidateScreenSpaceData CalculateScreenSpaceData(const APlayerController* PlayerController, const AActor* TargetActor, bool bScore);
	static bool HasLineOfSight(const AActor* TargetingActor, const AActor* Target);
	static bool GetCameraDistance(const FVector& TargetLocation, const FVector& CameraLocation, const FVector& CameraForward, float& OutCameraDistance);

private:
	static TArray<const AActor*> EMPTY_IGNORE;
};
