// Copyright Alien Shores


#include "Player/Form/PlayerFormAttributeModifiers.h"

#include "ElectricCastle/ElectricCastleLogChannels.h"

float UPlayerFormAttributeModifiers::GetAttributeModifier(const FGameplayTag& AttributeTag) const
{
	const FPlayerFormAttributeModifierEntry* Entry = AttributeModifiers.FindByPredicate([AttributeTag](const FPlayerFormAttributeModifierEntry& Entry)
	{
		return Entry.AttributeTag == AttributeTag;
	});
	if (Entry && Entry->IsValid())
	{
		return Entry->Modifier;
	}
	UE_LOG(LogElectricCastle, Warning, TEXT("[%s] No attribute modifier configured for tag %s"), *GetName(), *AttributeTag.ToString());
	return -1;
}
