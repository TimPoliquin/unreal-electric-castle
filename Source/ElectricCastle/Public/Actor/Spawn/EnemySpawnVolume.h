// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "SpawnEventPayloadDefinitions.h"
#include "Game/Save/SaveableInterface.h"
#include "GameFramework/Actor.h"
#include "Interaction/SaveInterface.h"
#include "EnemySpawnVolume.generated.h"

class UActorTrackerComponent;
class AEnemySpawnPoint;
class UBoxComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAuraEnemySpawnVolumeAllEnemiesDefeatedSignature);

UCLASS()
class ELECTRICCASTLE_API AEnemySpawnVolume : public AActor, public ISaveableInterface
{
	GENERATED_BODY()

public:
	AEnemySpawnVolume();
	void SetEnabled(const bool bInEnabled);

	/** Start ISaveableInterface **/
	virtual void PostLoad_Implementation() override;
	/** End ISaveableInterface **/
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAuraEnemySpawnVolumeAllEnemiesDefeatedSignature OnAllEnemiesDefeated;

protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void OnBoxOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category="Properties|Debug")
	bool bEnabled = true;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category="Properties|Debug")
	bool bTriggered = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UActorTrackerComponent> ActorTrackerComponent;

private:
	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<UBoxComponent> Box;
	UPROPERTY(EditAnywhere, Category="Properties")
	TArray<AEnemySpawnPoint*> SpawnPoints;

	UFUNCTION()
	void OnActorTrackerCountChangedHandler(const FOnActorTrackerCountChangedPayload& Payload);
	void AddChildSpawnPoints();
};
