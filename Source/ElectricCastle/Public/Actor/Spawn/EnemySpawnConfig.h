#pragma once

#include "CoreMinimal.h"
#include "EnemySpawnConfig.generated.h"

class AElectricCastleEnemyCharacter;

USTRUCT(BlueprintType)
struct FEnemySpawnConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn Config")
	TSubclassOf<AElectricCastleEnemyCharacter> EnemyClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn Config")
	int32 EnemyLevel = 1;

	bool IsValid() const;

	/** Spawn and fully initialize an enemy. **/
	AElectricCastleEnemyCharacter* Spawn(const AActor* SpawnPoint) const;
	/** Spawn and partially initialize an enemy. The caller is responsible for finishing spawning and setting the controller. **/
	AElectricCastleEnemyCharacter* SpawnDeferred(const AActor* SpawnPoint) const;
};
