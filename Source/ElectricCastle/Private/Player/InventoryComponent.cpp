// Copyright Alien Shores


#include "Player/InventoryComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Game/ElectricCastleGameState.h"
#include "Game/Save/SaveGameBlueprintFunctionLibrary.h"
#include "Game/Subsystem/ElectricCastleGameDataSubsystem.h"

UInventoryComponent* UInventoryComponent::Get(const UObject* InObject)
{
	if (const AElectricCastleGameState* GameState = AElectricCastleGameState::Get(InObject))
	{
		return GameState->GetInventoryComponent();
	}
	return nullptr;
}

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SaveID = USaveGameBlueprintFunctionLibrary::GenerateSaveID(this);
	SetIsReplicatedByDefault(true);
}

TArray<uint8> UInventoryComponent::SaveData_Implementation()
{
	return SerializeComponentData();
}

bool UInventoryComponent::LoadData_Implementation(const TArray<uint8>& Data)
{
	return DeserializeComponentData(Data);
}

bool UInventoryComponent::ShouldSave_Implementation() const
{
	return true;
}

FString UInventoryComponent::GetSaveID_Implementation() const
{
	return SaveID;
}

bool UInventoryComponent::HasItemInInventory(const FGameplayTag& ItemType) const
{
	return Inventory.ContainsByPredicate(
		[ItemType](const FItemInventoryEntry& Entry)
		{
			return Entry.ItemType == ItemType && Entry.ItemCount > 0;
		}
	);
}

int32 UInventoryComponent::AddToInventory(const FGameplayTag& ItemType, const int32 Count)
{
	const FItemDefinition ItemDefinition = UElectricCastleGameDataSubsystem::Get(GetOwner())->FindItemDefinitionByItemTag(ItemType);
	FItemInventoryEntry* ItemEntry = Inventory.FindByPredicate(
		[ItemType](const FItemInventoryEntry& Entry)
		{
			return Entry.ItemType.MatchesTagExact(ItemType);
		}
	);
	const bool AddToInventory = !ItemEntry;
	if (!ItemEntry)
	{
		if (Inventory.Num() + ItemDefinition.InventorySize <= MaxItems)
		{
			ItemEntry = new FItemInventoryEntry();
			ItemEntry->ItemType = ItemType;
			ItemEntry->ItemCount = 0;
		}
		else
		{
			OnInventoryFullDelegate.Broadcast(ItemType);
			return 0;
		}
	}
	if (ItemEntry->ItemCount >= ItemDefinition.InventoryMaxCount)
	{
		// This item is already at max capacity in the inventory
		OnInventoryFullDelegate.Broadcast(ItemType);
		return 0;
	}
	const int32 CountToAdd = FMath::Min(Count, ItemDefinition.InventoryMaxCount - ItemEntry->ItemCount);
	const int32 OldValue = ItemEntry->ItemCount;
	const int32 NewValue = ItemEntry->ItemCount + CountToAdd;
	ItemEntry->ItemCount = NewValue;
	if (AddToInventory)
	{
		Inventory.Add(*ItemEntry);
	}
	OnInventoryItemCountChangedDelegate.Broadcast(FOnInventoryItemCountChangedPayload(
		ItemType,
		OldValue,
		NewValue
	));
	return CountToAdd;
}

bool UInventoryComponent::UseConsumable(const FGameplayTag& ItemType)
{
	return UseItem(ItemType, EItemCategory::Consumable);
}

bool UInventoryComponent::UseKey(const FGameplayTag& ItemType)
{
	return UseItem(ItemType, EItemCategory::Key);
}

TArray<FItemInventoryEntry> UInventoryComponent::GetInventory() const
{
	return Inventory;
}

bool UInventoryComponent::UseItem(const FGameplayTag& ItemTag, const EItemCategory& ItemCategory)
{
	const FItemDefinition ItemDefinition = UElectricCastleGameDataSubsystem::Get(GetOwner())->FindItemDefinitionByItemTag(ItemTag);
	if (!ItemDefinition.IsValid())
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s][%s] Attempted to find item that has no definition: %s"), *GetOwner()->GetName(), *GetName(), *ItemTag.ToString());
		return false;
	}
	if (ItemDefinition.ItemCategory != ItemCategory)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s][%s] Attempted to use item of incorrect category: %s != %s"), *GetOwner()->GetName(), *GetName(), *UEnum::GetValueAsString(ItemCategory),
		       *UEnum::GetValueAsString(ItemDefinition.ItemCategory));
		return false;
	}
	FItemInventoryEntry* ItemEntry = Inventory.FindByPredicate(
		[ItemTag](const FItemInventoryEntry& Entry)
		{
			return Entry.ItemType.MatchesTagExact(ItemTag);
		}
	);
	if (!ItemEntry || !ItemEntry->IsValid())
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s][%s] Item not found in inventory: %s"), *GetOwner()->GetName(), *GetName(), *ItemTag.ToString());
		return false;
	}
	if (ItemEntry->ItemCount <= 0)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s][%s] Item found in inventory but with 0 count: %s"), *GetOwner()->GetName(), *GetName(), *ItemTag.ToString());
		return false;
	}
	const int32 OldValue = ItemEntry->ItemCount;
	const int32 NewValue = ItemEntry->ItemCount - 1;
	ItemEntry->ItemCount = NewValue;
	if (ItemEntry->ItemCount <= 0)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s][%s] Used item %s"), *GetOwner()->GetName(), *GetName(), *ItemTag.ToString())
		Inventory.RemoveAll([ItemTag](const FItemInventoryEntry& Entry)
		{
			return Entry.ItemType.MatchesTagExact(ItemTag);
		});
	}
	OnInventoryItemCountChangedDelegate.Broadcast(FOnInventoryItemCountChangedPayload(
		ItemTag,
		OldValue,
		NewValue
	));
	return true;
}

TArray<uint8> UInventoryComponent::SerializeComponentData() const
{
	TArray<uint8> Data;
	FMemoryWriter Writer(Data);

	// Create save data struct
	FInventoryComponentSaveData SaveData;
	SaveData.MaxItems = MaxItems;
	SaveData.InventoryItems = Inventory;

	// Serialize the struct
	Writer << SaveData.MaxItems;
	Writer << SaveData.InventoryItems;

	return Data;
}

bool UInventoryComponent::DeserializeComponentData(const TArray<uint8>& Data)
{
	if (Data.Num() == 0)
	{
		return false;
	}

	FMemoryReader Reader(Data);

	try
	{
		Reader << MaxItems;
		Reader << Inventory;

		return true;
	}
	catch (...)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s:%s] Failed to deserialize data"), *GetOwner()->GetName(), *GetName());
		return false;
	}
}
