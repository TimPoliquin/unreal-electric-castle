// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Actor/POI/PointOfInterestActor.h"
#include "Interaction/HighlightInterface.h"
#include "Item/ItemTypes.h"
#include "TreasureChestActor.generated.h"


class ULootSpawnComponent;
struct FOnLockComponentUnlockPayload;
class ATreasurePickup;
class AItemPickupBase;
class ULockComponent;

UENUM(BlueprintType)
enum class EAuraTreasureChestState : uint8
{
	Locked,
	Unlocked,
	Open
};

UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API ATreasureChestActor : public APointOfInterestActor, public IHighlightInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATreasureChestActor();
	virtual void HandleInteract_Implementation(AActor* Player) override;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsUnlocked() const;
	void HandleInitialState();
	virtual bool SetMoveToLocation_Implementation(FVector& OutDestination) override;
	/** Start ISaveInterface **/
	virtual void PostLoad_Implementation() override;
	/** End ISaveInterface **/


protected:
	virtual void BeginPlay() override;
	virtual bool IsPreconditionMet_Implementation(AActor* Player) const override;
	void Unlock(AActor* Player);
	void Open(AActor* Player);
	UFUNCTION(BlueprintCallable)
	void GrantRewards();
	UFUNCTION(BlueprintNativeEvent)
	void PlayUnlockEffect();
	UFUNCTION(BlueprintNativeEvent)
	void PlayUnlockForcedEffect();
	UFUNCTION(BlueprintNativeEvent)
	void PlayOpenEffect(AActor* Player);
	UFUNCTION(BlueprintNativeEvent)
	void PlayOpenForcedEffect();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void GrantRewards_Finish();
	UFUNCTION(BlueprintNativeEvent)
	FTransform GetRewardInitialSpawnLocation() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<ULockComponent> LockComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<ULootSpawnComponent> LootSpawnComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category="Items")
	EAuraTreasureChestState State = EAuraTreasureChestState::Locked;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Items")
	TObjectPtr<USoundBase> OpenSound;

private:
	UFUNCTION()
	void OnChestUnlocked(const FOnLockComponentUnlockPayload& Payload);
};
