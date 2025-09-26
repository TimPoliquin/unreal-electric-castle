// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MVVMViewModelBase.h"
#include "UObject/Object.h"
#include "MVVM_InventoryItem.generated.h"

class AElectricCastlePlayerCharacter;
/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UMVVM_InventoryItem : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	void InitializeDependencies();
	FGameplayTag GetInventoryItemTag() const;
	void SetInventoryItemTag(const FGameplayTag& InInventoryItemTag);
	FString GetInventoryItemName() const;
	void SetInventoryItemName(const FString& InInventoryItemName);
	int32 GetQuantity() const;
	void SetQuantity(const int32 InQuantity);
	int32 GetItemIndex() const;
	void SetItemIndex(const int32 InItemIndex);
	int32 GetRow() const;
	void SetRow(const int32 InRow);
	int32 GetColumn() const;
	void SetColumn(const int32 InColumn);
	bool GetIsEquipped() const;
	void SetIsEquipped(const bool InIsEquipped);
	bool IsEmpty() const;
	void Clear();
	UTexture2D* GetIcon() const;
	void SetIcon(UTexture2D* InIcon);

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> DefaultEmptyIcon;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	FGameplayTag InventoryItemTag = FGameplayTag::EmptyTag;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	FString InventoryItemName;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	UTexture2D* Icon;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	int32 Quantity = 0;
	UPROPERTY(BlueprintReadOnly, FieldNotify, Setter, Getter)
	bool IsEquipped = false;
	UPROPERTY(BlueprintReadOnly)
	int32 ItemIndex = -1;
	UPROPERTY(BlueprintReadOnly)
	int32 Row = -1;
	UPROPERTY(BlueprintReadOnly)
	int32 Column = -1;

private:
	void UpdateFromInventoryItemTag();
};
