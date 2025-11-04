// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "ElectricCastleActorBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UElectricCastleActorBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void SinusoidalMovement(
		AActor* Actor,
		float DeltaSeconds,
		float& RunningTime,
		float PeriodMultiplier,
		float Amplitude,
		const FVector& InitialLocation
	);

	static void DisableActor(AActor* Actor);

	static UShapeComponent* FindCollisionComponent(const AActor* Actor);
	static UMeshComponent* FindMeshComponent(const AActor* Actor);
};
