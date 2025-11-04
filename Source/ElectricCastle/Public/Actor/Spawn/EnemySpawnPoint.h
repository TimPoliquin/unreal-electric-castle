// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "EnemySpawnConfig.h"
#include "Engine/TargetPoint.h"
#include "EnemySpawnPoint.generated.h"

class AElectricCastleEnemyCharacter;

UCLASS()
class ELECTRICCASTLE_API AEnemySpawnPoint : public ATargetPoint
{
	GENERATED_BODY()

public:
	AEnemySpawnPoint();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemey Class")
	FEnemySpawnConfig SpawnConfig;

	UFUNCTION(BlueprintCallable, Category = "Enemey Class")
	AElectricCastleEnemyCharacter* SpawnEnemy() const;
};
