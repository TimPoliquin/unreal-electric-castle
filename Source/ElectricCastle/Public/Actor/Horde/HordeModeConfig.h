// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Actor/Spawn/EnemySpawnConfig.h"
#include "Engine/DataAsset.h"
#include "Item/Pickup/ItemPickupBase.h"
#include "Utils/RandUtils.h"
#include "HordeModeConfig.generated.h"

struct FAuraSpawnParams;
class AEnemySpawnTunnel;
class AElectricCastleEnemyCharacter;

USTRUCT(BlueprintType)
struct FHordeRoundRewards
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AItemPickupBase> RewardClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Count = 1;
};

USTRUCT(BlueprintType)
struct FHordeRoundConfig
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 NumSpawnPoints = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 NumEnemiesToSpawn = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FRandRange SpawnDelay;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FEnemySpawnConfig> EnemyConfig;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FHordeRoundRewards> Rewards;
};

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UHordeModeConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FHordeRoundConfig GetRoundConfig(const int32 RoundNum) const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasConfigurationForRound(const int32 RoundNum) const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<FHordeRoundRewards> GetRewards(const int32 RoundNum) const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static int32 GetNumberOfRewardsToSpawn(TArray<FHordeRoundRewards> Rewards);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<FHordeRoundConfig> RoundConfigs;
};
