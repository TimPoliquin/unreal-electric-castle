// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "SpawnEventPayloadDefinitions.h"
#include "UObject/Interface.h"
#include "SpawnInterface.generated.h"

struct FRandRange;
struct FEnemySpawnConfig;
class AElectricCastleEnemyCharacter;
// This class does not need to be modified.
UINTERFACE()
class USpawnInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API ISpawnInterface
{
	GENERATED_BODY()

public:
	virtual FOnAuraActorTrackerCountChangedDelegate& GetOnCountChangedDelegate() = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void BeginSpawning(const int32 NumEnemies, const FRandRange& SpawnDelay, const TArray<FEnemySpawnConfig>& SpawnClasses);
};
