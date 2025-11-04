// Copyright Alien Shores


#include "Item/Data/ItemDefinitions.h"
#include "Item/ItemTypes.h"

#include "ElectricCastle/ElectricCastleLogChannels.h"

FItemDefinition UItemDefinitions::FindItemByItemType(const FGameplayTag& ItemType) const
{
	for (const FItemDefinition& ItemDefinition : ItemInfo)
	{
		if (ItemDefinition.ItemType == ItemType)
		{
			return ItemDefinition;
		}
	}
	return FItemDefinition();
}

void UItemDefinitions::AddToMap(TMap<FGameplayTag, FItemDefinition>& ItemDefinitionMap) const
{
	for (const FItemDefinition& ItemDefinition : ItemInfo)
	{
		if (ItemDefinitionMap.Contains(ItemDefinition.ItemType))
		{
			UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Duplicate item definition detected: %s"), *GetName(), *ItemDefinition.ItemType.ToString());
		}
		else
		{
			ItemDefinitionMap.Add(ItemDefinition.ItemType, ItemDefinition);
		}
	}
}
