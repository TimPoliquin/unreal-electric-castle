// Copyright Alien Shores


#include "UI/ViewModel/MVVM_InventoryItem.h"

#include "Item/ItemBlueprintLibrary.h"

void UMVVM_InventoryItem::InitializeDependencies()
{
	SetIcon(DefaultEmptyIcon);
}

FGameplayTag UMVVM_InventoryItem::GetInventoryItemTag() const
{
	return InventoryItemTag;
}

void UMVVM_InventoryItem::SetInventoryItemTag(const FGameplayTag& InInventoryItemTag)
{
	UE_MVVM_SET_PROPERTY_VALUE(InventoryItemTag, InInventoryItemTag);
	UpdateFromInventoryItemTag();
}

int32 UMVVM_InventoryItem::GetQuantity() const
{
	return Quantity;
}

void UMVVM_InventoryItem::SetQuantity(const int32 InQuantity)
{
	UE_MVVM_SET_PROPERTY_VALUE(Quantity, InQuantity);
}

bool UMVVM_InventoryItem::GetIsEquipped() const
{
	return IsEquipped;
}

void UMVVM_InventoryItem::SetIsEquipped(const bool InIsEquipped)
{
	UE_MVVM_SET_PROPERTY_VALUE(IsEquipped, InIsEquipped);
}

UTexture2D* UMVVM_InventoryItem::GetIcon() const
{
	return Icon;
}

void UMVVM_InventoryItem::SetIcon(UTexture2D* const InIcon)
{
	UE_MVVM_SET_PROPERTY_VALUE(Icon, InIcon);
}

FString UMVVM_InventoryItem::GetInventoryItemName() const
{
	return InventoryItemName;
}

void UMVVM_InventoryItem::SetInventoryItemName(const FString& InInventoryItemName)
{
	UE_MVVM_SET_PROPERTY_VALUE(InventoryItemName, InInventoryItemName);
}

int32 UMVVM_InventoryItem::GetItemIndex() const
{
	return ItemIndex;
}

void UMVVM_InventoryItem::SetItemIndex(const int32 InItemIndex)
{
	ItemIndex = InItemIndex;
}

int32 UMVVM_InventoryItem::GetRow() const
{
	return Row;
}

void UMVVM_InventoryItem::SetRow(const int32 InRow)
{
	this->Row = InRow;
}

int32 UMVVM_InventoryItem::GetColumn() const
{
	return Column;
}

void UMVVM_InventoryItem::SetColumn(const int32 InColumn)
{
	this->Column = InColumn;
}

bool UMVVM_InventoryItem::IsEmpty() const
{
	return !InventoryItemTag.IsValid() || Quantity <= 0;
}

void UMVVM_InventoryItem::Clear()
{
	SetInventoryItemTag(FGameplayTag::EmptyTag);
	SetQuantity(0);
	SetIsEquipped(false);
}


void UMVVM_InventoryItem::UpdateFromInventoryItemTag()
{
	if (InventoryItemTag.IsValid())
	{
		const FItemDefinition& ItemDef = UItemBlueprintLibrary::GetItemDefinitionByItemType(GetWorld(), InventoryItemTag);
		SetInventoryItemName(ItemDef.ItemName);
		SetIcon(ItemDef.ItemIcon);
	}
	else
	{
		SetInventoryItemName(FString(""));
		SetIcon(DefaultEmptyIcon);
	}
}
