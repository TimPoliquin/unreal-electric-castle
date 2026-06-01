// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "SpawnInterface.h"
#include "AI/Alert/AlertTypes.h"
#include "GameFramework/Actor.h"
#include "SpawnManager.generated.h"

class UActorTrackerComponent;

UCLASS(Abstract)
class ELECTRICCASTLE_API ASpawnManager : public AActor, public ISpawnInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASpawnManager();

	/** Start Spawn Interface **/
	virtual FActorTrackerCountChangedDelegate& GetOnCountChangedDelegate() override;
	virtual void BeginSpawning_Implementation(const int32 NumEnemies, const FRandRange& SpawnDelay, const TArray<FEnemySpawnConfig>& SpawnClasses) override;
	virtual int32 GetSpawnCount_Implementation() const override;
	/** End Spawn Interface **/
	UPROPERTY(BlueprintAssignable)
	FAllSpawnsDestroyedDelegate OnAllSpawnsDestroyedDelegate;

protected:
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable)
	void InitializeEnemies(const int32 NumToSpawn, const TArray<FEnemySpawnConfig>& SpawnClasses, TArray<AElectricCastleEnemyCharacter*>& OutEnemies);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SpawnEnemiesWithDelay(const FRandRange& SpawnDelay);
	UFUNCTION(BlueprintCallable)
	virtual void FinishSpawningEnemy(AElectricCastleEnemyCharacter* Enemy);
	UFUNCTION(BlueprintNativeEvent)
	void OnEnemyCountChanged(const FOnActorTrackerCountChangedPayload& Payload);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnAllSpawnsDestroyed();


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UActorTrackerComponent> EnemyTrackerComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UChildActorComponent> SpawnPointComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bOverrideAlertLevel = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition=bOverrideAlertLevel, EditConditionHides))
	EAlertLevel AlertLevelOverride = EAlertLevel::Alerted;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition=bOverrideAlertLevel, EditConditionHides))
	bool bPinAlertLevel = true;

private:
	UFUNCTION()
	void OnSummonCountChanged(const FOnActorTrackerCountChangedPayload& Payload);
	UPROPERTY()
	FTimerHandle EnemySpawnTimer;
	UPROPERTY()
	TArray<AElectricCastleEnemyCharacter*> Enemies;
};
