#include "Fishing/FishTypes.h"

#include "Fishing/FishCatch.h"
#include "Player/InventoryComponent.h"

void UFishCatch::AddToPlayerInventory(AActor* Player)
{
	if (UInventoryComponent* InventoryComponent = UInventoryComponent::Get(Player))
	{
		InventoryComponent->AddToInventory(FishType);
	}
}
