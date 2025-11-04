// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "SpawnEventPayloadDefinitions.h"
#include "SpawnInterface.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "EnemySpawnTunnel.generated.h"

class UTimelineComponent;
struct FEnemySpawnConfig;
class AEnemySpawnTunnel;
class UActorTrackerComponent;
class AElectricCastleEnemyCharacter;

USTRUCT(BlueprintType)
struct FOnAuraSpawnTunnelDefeatedPayload
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AEnemySpawnTunnel> SpawnTunnel;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAuraSpawnTunnelDefeatedSignature, const FOnAuraSpawnTunnelDefeatedPayload&, Payload);

UCLASS()
class ELECTRICCASTLE_API AEnemySpawnTunnel : public AActor, public ISpawnInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEnemySpawnTunnel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Start AuraSpawnInterface **/
	virtual FOnAuraActorTrackerCountChangedDelegate& GetOnCountChangedDelegate() override;
	virtual void BeginSpawning_Implementation(const int32 NumEnemies, const FRandRange& SpawnDelay, const TArray<FEnemySpawnConfig>& SpawnClasses) override;

	/** End AuraSpawnInterface **/

	UPROPERTY(BlueprintAssignable)
	FOnAuraSpawnTunnelDefeatedSignature OnSpawnTunnelDefeated;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UActorTrackerComponent> EnemyTrackerComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UChildActorComponent> SpawnPointComponent;
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

	UFUNCTION(BlueprintCallable)
	TArray<AElectricCastleEnemyCharacter*> InitializeEnemies(int32 NumToSpawn, const TArray<FEnemySpawnConfig>& SpawnClasses);
	UFUNCTION(BlueprintCallable)
	void FinishSpawningEnemy(AElectricCastleEnemyCharacter* Enemy);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PlayLightingEffect();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PlayLightingEffect_Tick(const float Intensity);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PlayStartSound();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PlayEndSound();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SpawnEnemiesWithDelay(TArray<AElectricCastleEnemyCharacter*>& Enemies, const FRandRange& SpawnDelay);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void DeactivateSpawnTunnel();

private:
	UFUNCTION()
	void OnEnemyCountChanged(const FOnActorTrackerCountChangedPayload& Payload);
	float LightBaseIntensity = 0.f;
	FTimerHandle EnemySpawnTimer;
	FOnTimelineFloat OnLightingTimelineTickDelegate;
};
