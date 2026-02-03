// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "SpawnInterface.h"
#include "SpawnManager.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "EnemySpawnTunnel.generated.h"

class UTimelineComponent;
struct FEnemySpawnConfig;
class AEnemySpawnTunnel;
class UActorTrackerComponent;
class AElectricCastleEnemyCharacter;

UCLASS()
class ELECTRICCASTLE_API AEnemySpawnTunnel : public ASpawnManager
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEnemySpawnTunnel();
	/** Start SpawnInterface **/
	virtual void BeginSpawning_Implementation(const int32 NumEnemies, const FRandRange& SpawnDelay, const TArray<FEnemySpawnConfig>& SpawnClasses) override;
	/** End SpawnInterface **/

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnAllSpawnsDestroyed_Implementation() override;
	virtual void FinishSpawningEnemy(AElectricCastleEnemyCharacter* Enemy) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<ULightComponent> LightComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USoundBase> StartSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USoundBase> EndSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UCurveFloat> LightingEffectCurve;
	UPROPERTY()
	TObjectPtr<UTimelineComponent> TimelineComponent;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PlayLightingEffect();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PlayLightingEffect_Tick(const float Intensity);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PlayStartSound();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PlayEndSound();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void DeactivateSpawnTunnel();

private:
	float LightBaseIntensity = 0.f;
	FOnTimelineFloat OnLightingTimelineTickDelegate;
};
