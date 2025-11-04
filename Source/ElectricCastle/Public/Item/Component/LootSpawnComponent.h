// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/ItemTypes.h"
#include "LootSpawnComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLootGrantCompletedSignature);


UENUM(BlueprintType)
enum class EItemGrantMode : uint8
{
	Spawn,
	DirectToInventory
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API ULootSpawnComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULootSpawnComponent();

	UPROPERTY(BlueprintAssignable)
	FOnLootGrantCompletedSignature OnLootGrantCompleted;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void GrantLoot();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Items")
	EItemGrantMode GrantMode = EItemGrantMode::Spawn;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Items")
	TArray<FLootDefinition> LootDefinitions;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Items", meta=(MustImplement="/Script/Aura.SpawnEffectInterface"))
	TSubclassOf<AActor> RewardAnimatorClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Items")
	int32 GoldAmount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Items")
	float SpawnDistance = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Items")
	float SpawnRadius = 120.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Items")
	float HeightMultiplier = 1.f;

	UFUNCTION(BlueprintNativeEvent)
	void AddLootToInventory();
	UFUNCTION(BlueprintNativeEvent)
	void SpawnLoot();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InstantiateLootActors(TArray<FLootInstance>& OutLootInstances);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FTransform GetInitialLootSpawnTransform();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PlaySpawnItems();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PlaySpawnItem(const FLootInstance& LootInstance);

private:
	FTimerHandle SpawnDelayTimer;
	UPROPERTY()
	TArray<FLootInstance> LootInstances;
};
