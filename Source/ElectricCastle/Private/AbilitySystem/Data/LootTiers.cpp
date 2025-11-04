// Copyright Alien Shores


#include "AbilitySystem/Data/LootTiers.h"

TArray<FLootItem> ULootTiers::GetLootItems() const
{
	TArray<FLootItem> Items;
	for (const FLootItem& LootItem : LootItems)
	{
		for (int32 i = 0; i < LootItem.MaxNumberToSpawn; i++)
		{
			if (FMath::FRandRange(0, 100.f) < LootItem.ChanceToSpawn)
			{
				FLootItem NewItem;
				NewItem.LootClass = LootItem.LootClass;
				NewItem.bLootLevelOverride = LootItem.bLootLevelOverride;
				Items.Add(NewItem);
			}
		}
	}
	return Items;
}
