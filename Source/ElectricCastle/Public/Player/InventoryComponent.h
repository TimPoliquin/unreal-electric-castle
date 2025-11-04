// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/ItemTypes.h"
#include "InventoryEvents.h"
#include "Game/Save/SaveableInterface.h"
#include "InventoryComponent.generated.h"

class AFishingRodActor;
class UGameplayEffect;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryFullSignature, const FGameplayTag&, ItemType);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryItemCountChangedSignature, const FOnInventoryItemCountChangedPayload&, Payload);

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FInventoryComponentSaveData
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	int32 MaxItems = 0;
	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	TArray<FItemInventoryEntry> InventoryItems;
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API UInventoryComponent : public UActorComponent, public ISaveableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="PlayerInventoryComponent|Utils")
	static UInventoryComponent* Get(const UObject* InObject);

	UInventoryComponent();

	/** Start ISaveableInterface **/
	virtual TArray<uint8> SaveData_Implementation() override;
	virtual bool LoadData_Implementation(const TArray<uint8>& Data) override;
	virtual bool ShouldSave_Implementation() const override;
	virtual bool ShouldLoadTransform_Implementation() const override { return false; }
	virtual bool ShouldAutoSpawn_Implementation() const override { return false; }
	virtual FString GetSaveID_Implementation() const override;
	/** Start ISaveableInterface **/


	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasItemInInventory(const FGameplayTag& ItemType) const;
	UFUNCTION(BlueprintCallable, Category="Item")
	int32 AddToInventory(const FGameplayTag& ItemType, int32 Count = 1);
	UFUNCTION(BlueprintCallable, Category="Item")
	bool UseConsumable(const FGameplayTag& ItemType);
	UFUNCTION(BlueprintCallable, Category="Item")
	bool UseKey(const FGameplayTag& ItemType);

	UPROPERTY(BlueprintAssignable)
	FOnInventoryItemCountChangedSignature OnInventoryItemCountChangedDelegate;
	UPROPERTY(BlueprintAssignable)
	FOnInventoryFullSignature OnInventoryFullDelegate;

	TArray<FItemInventoryEntry> GetInventory() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	int32 MaxItems = 25;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	TArray<FItemInventoryEntry> Inventory;

private:
	FString SaveID;
	bool UseItem(const FGameplayTag& ItemTag, const EItemCategory& ItemCategory);
	TArray<uint8> SerializeComponentData() const;
	bool DeserializeComponentData(const TArray<uint8>& Data);
};
