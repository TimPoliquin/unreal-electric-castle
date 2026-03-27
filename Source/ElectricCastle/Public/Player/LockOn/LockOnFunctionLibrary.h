// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LockOnFunctionLibrary.generated.h"

USTRUCT()
struct FCandidateEntry
{
	GENERATED_BODY()
	UPROPERTY()
	TObjectPtr<AActor> Target;
	UPROPERTY()
	float Distance = FLT_MAX;
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
	static AActor* FindClosestTarget(const AActor* TargetingActor, float LockOnRadius, float MaxLockOnDistance, bool bRequireLineOfSight = true);
	UFUNCTION(BlueprintCallable)
	static AActor* SwitchTarget(
		const AActor* TargetingActor,
		const AActor* CurrentTarget,
		const FVector2D& StickInput,
		const FRotator& CameraRotation,
		float LockOnRadius,
		float MaxLockOnDistance,
		bool bRequireLineOfSight,
		bool bDebug = true
	);
	static bool IsTargetValid(const AActor* Target);

protected:
	static TArray<FCandidateEntry> GatherCandidates(const AActor* TargetingActor, float TargetingRadius, float MaxLockOnDistance, bool bRequireLineOfSight);
	static bool HasLineOfSight(const AActor* TargetingActor, const AActor* Target);
};
