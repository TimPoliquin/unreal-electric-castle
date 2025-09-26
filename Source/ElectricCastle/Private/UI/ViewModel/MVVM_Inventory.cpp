// Copyright Alien Shores


#include "UI/ViewModel/MVVM_Inventory.h"

#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Character/ElectricCastlePlayerCharacter.h"
#include "Game/ElectricCastleGameState.h"
#include "Player/InventoryComponent.h"
#include "UI/ViewModel/MVVM_InventoryItem.h"

FString UMVVM_Inventory::GetInventoryName() const
{
	return InventoryName;
}

void UMVVM_Inventory::SetInventoryName(const FString& InInventoryName)
{
	UE_MVVM_SET_PROPERTY_VALUE(InventoryName, InInventoryName);
}

void UMVVM_Inventory::InitializeInventoryItems()
{
	InventoryItems.SetNum(5 * 5); // 5 rows, 5 columns
	InventoryItem_0_0 = CreateInventoryItemViewModel(0, 0);
	InventoryItem_0_1 = CreateInventoryItemViewModel(0, 1);
	InventoryItem_0_2 = CreateInventoryItemViewModel(0, 2);
	InventoryItem_0_3 = CreateInventoryItemViewModel(0, 3);
	InventoryItem_0_4 = CreateInventoryItemViewModel(0, 4);
	InventoryItem_1_0 = CreateInventoryItemViewModel(1, 0);
	InventoryItem_1_1 = CreateInventoryItemViewModel(1, 1);
	InventoryItem_1_2 = CreateInventoryItemViewModel(1, 2);
	InventoryItem_1_3 = CreateInventoryItemViewModel(1, 3);
	InventoryItem_1_4 = CreateInventoryItemViewModel(1, 4);
	InventoryItem_2_0 = CreateInventoryItemViewModel(2, 0);
	InventoryItem_2_1 = CreateInventoryItemViewModel(2, 1);
	InventoryItem_2_2 = CreateInventoryItemViewModel(2, 2);
	InventoryItem_2_3 = CreateInventoryItemViewModel(2, 3);
	InventoryItem_2_4 = CreateInventoryItemViewModel(2, 4);
	InventoryItem_3_0 = CreateInventoryItemViewModel(3, 0);
	InventoryItem_3_1 = CreateInventoryItemViewModel(3, 1);
	InventoryItem_3_2 = CreateInventoryItemViewModel(3, 2);
	InventoryItem_3_3 = CreateInventoryItemViewModel(3, 3);
	InventoryItem_3_4 = CreateInventoryItemViewModel(3, 4);
	InventoryItem_4_0 = CreateInventoryItemViewModel(4, 0);
	InventoryItem_4_1 = CreateInventoryItemViewModel(4, 1);
	InventoryItem_4_2 = CreateInventoryItemViewModel(4, 2);
	InventoryItem_4_3 = CreateInventoryItemViewModel(4, 3);
	InventoryItem_4_4 = CreateInventoryItemViewModel(4, 4);
}

UMVVM_InventoryItem* UMVVM_Inventory::CreateInventoryItemViewModel(const int32 Row, const int32 Column)
{
	const int32 ItemIdx = Row * Columns + Column;
	UMVVM_InventoryItem* InventoryItem = NewObject<UMVVM_InventoryItem>(this, InventoryItemViewModelClass);
	InventoryItem->SetItemIndex(ItemIdx);
	InventoryItem->SetRow(Row);
	InventoryItem->SetColumn(Column);
	InventoryItems[ItemIdx] = InventoryItem;
	return InventoryItem;
}

void UMVVM_Inventory::InitializeDependencies()
{
	for (int32 Idx = 0; Idx < InventoryItems.Num(); ++Idx)
	{
		InventoryItems[Idx]->InitializeDependencies();
	}
	if (UInventoryComponent* PlayerInventoryComponent = UInventoryComponent::Get(GetWorld()))
	{
		PlayerInventoryComponent->OnInventoryItemCountChangedDelegate.AddUniqueDynamic(this, &UMVVM_Inventory::OnPlayerInventoryChanged);
		for (auto InventoryItem : PlayerInventoryComponent->GetInventory())
		{
			if (InventoryItem.IsValid())
			{
				OnPlayerInventoryChanged(FOnInventoryItemCountChangedPayload(InventoryItem.ItemType, 0, InventoryItem.ItemCount));
			}
		}
	}
}

TArray<UMVVM_InventoryItem*> UMVVM_Inventory::GetInventoryItems()
{
	return InventoryItems;
}

void UMVVM_Inventory::OnPlayerInventoryChanged(const FOnInventoryItemCountChangedPayload& Payload)
{
	if (!Payload.IsValid())
	{
		return;
	}
	if (Payload.IsNewItemAddedChange())
	{
		UMVVM_InventoryItem* ItemModel = *InventoryItems.FindByPredicate([](const UMVVM_InventoryItem* InventoryItem)
		{
			return InventoryItem->IsEmpty();
		});
		ItemModel->SetInventoryItemTag(Payload.ItemType);
		ItemModel->SetQuantity(Payload.NewValue);
	}
	else
	{
		UMVVM_InventoryItem* ItemModel = *InventoryItems.FindByPredicate([Payload](const UMVVM_InventoryItem* InventoryItem)
		{
			return InventoryItem->GetInventoryItemTag().MatchesTagExact(Payload.ItemType);
		});
		if (!ItemModel)
		{
			UE_LOG(LogElectricCastle, Error, TEXT("[%s] No matching inventory item model found! ItemType: %s"), *GetName(), *Payload.ItemType.ToString());
			return;
		}
		if (Payload.NewValue <= 0)
		{
			ItemModel->Clear();
		}
		else if (Payload.NewValue > 0)
		{
			ItemModel->SetQuantity(Payload.NewValue);
		}
	}
}

int32 UMVVM_Inventory::GetRows() const
{
	return Rows;
}

int32 UMVVM_Inventory::GetColumns() const
{
	return Columns;
}
