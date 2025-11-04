// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Player/InventoryEvents.h"
#include "MVVM_Inventory.generated.h"

class UMVVM_InventoryItem;
/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UMVVM_Inventory : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	FString GetInventoryName() const;
	void SetInventoryName(const FString& InInventoryName);
	UFUNCTION(BlueprintCallable)
	void InitializeInventoryItems();
	UFUNCTION(BlueprintCallable)
	void InitializeDependencies();
	TArray<UMVVM_InventoryItem*> GetInventoryItems();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetRows() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetColumns() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess="true"))
	FString InventoryName = FString("Inventory");
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UMVVM_InventoryItem> InventoryItemViewModelClass;
	UPROPERTY(BlueprintReadOnly)
	TArray<UMVVM_InventoryItem*> InventoryItems;

private:
	UFUNCTION()
	void OnPlayerInventoryChanged(const FOnInventoryItemCountChangedPayload& Payload);

	const int32 Rows = 5;
	const int32 Columns = 5;

	UMVVM_InventoryItem* CreateInventoryItemViewModel(const int32 Row, const int32 Column);
	UPROPERTY()
	TObjectPtr<UMVVM_InventoryItem> InventoryItem_0_0;
	UPROPERTY()
	TObjectPtr<UMVVM_InventoryItem> InventoryItem_0_1;
	UPROPERTY()
	TObjectPtr<UMVVM_InventoryItem> InventoryItem_0_2;
	UPROPERTY()
	TObjectPtr<UMVVM_InventoryItem> InventoryItem_0_3;
	UPROPERTY()
	TObjectPtr<UMVVM_InventoryItem> InventoryItem_0_4;
	UPROPERTY()
	TObjectPtr<UMVVM_InventoryItem> InventoryItem_1_0;
	UPROPERTY()
	TObjectPtr<UMVVM_InventoryItem> InventoryItem_1_1;
	UPROPERTY()
	TObjectPtr<UMVVM_InventoryItem> InventoryItem_1_2;
	UPROPERTY()
	TObjectPtr<UMVVM_InventoryItem> InventoryItem_1_3;
	UPROPERTY()
	TObjectPtr<UMVVM_InventoryItem> InventoryItem_1_4;
	UPROPERTY()
	TObjectPtr<UMVVM_InventoryItem> InventoryItem_2_0;
	UPROPERTY()
	TObjectPtr<UMVVM_InventoryItem> InventoryItem_2_1;
	UPROPERTY()
	TObjectPtr<UMVVM_InventoryItem> InventoryItem_2_2;
	UPROPERTY()
	TObjectPtr<UMVVM_InventoryItem> InventoryItem_2_3;
	UPROPERTY()
	TObjectPtr<UMVVM_InventoryItem> InventoryItem_2_4;
	UPROPERTY()
	TObjectPtr<UMVVM_InventoryItem> InventoryItem_3_0;
	UPROPERTY()
	TObjectPtr<UMVVM_InventoryItem> InventoryItem_3_1;
	UPROPERTY()
	TObjectPtr<UMVVM_InventoryItem> InventoryItem_3_2;
	UPROPERTY()
	TObjectPtr<UMVVM_InventoryItem> InventoryItem_3_3;
	UPROPERTY()
	TObjectPtr<UMVVM_InventoryItem> InventoryItem_3_4;
	UPROPERTY()
	TObjectPtr<UMVVM_InventoryItem> InventoryItem_4_0;
	UPROPERTY()
	TObjectPtr<UMVVM_InventoryItem> InventoryItem_4_1;
	UPROPERTY()
	TObjectPtr<UMVVM_InventoryItem> InventoryItem_4_2;
	UPROPERTY()
	TObjectPtr<UMVVM_InventoryItem> InventoryItem_4_3;
	UPROPERTY()
	TObjectPtr<UMVVM_InventoryItem> InventoryItem_4_4;
};
