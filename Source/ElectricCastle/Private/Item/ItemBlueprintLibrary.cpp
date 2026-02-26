// Copyright Alien Shores


#include "Item/ItemBlueprintLibrary.h"

#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Game/Subsystem/ElectricCastleGameDataSubsystem.h"

FItemDefinition UItemBlueprintLibrary::GetItemDefinitionByItemType(
	const UObject* WorldContextObject,
	const FGameplayTag& ItemType
)
{
	if (UElectricCastleGameDataSubsystem* GameDataSubsystem = UElectricCastleGameDataSubsystem::Get(WorldContextObject))
	{
		return GameDataSubsystem->FindItemDefinitionByItemTag(ItemType);
	}
	UE_LOG(
		LogElectricCastle,
		Error,
		TEXT("[%s] Attempted get item definition, but game mode was null!"),
		*FString("UItemBlueprintLibrary::GetItemDefinitionByItemType")
	);
	return FItemDefinition();
}

FString UItemBlueprintLibrary::GetItemNameByItemType(
	const UObject* WorldContextObject,
	const FGameplayTag& ItemType
)
{
	return GetItemDefinitionByItemType(WorldContextObject, ItemType).ItemName;
}

FString UItemBlueprintLibrary::Substitute(const FString& Message, const FMessageSubstitutions& MessageSubstitutions)
{
	FString Result = Message;
	for (const TPair<FString, FString>& Entry : MessageSubstitutions.Substitutions)
	{
		const FString Key = "{" + Entry.Key + "}";
		Result.ReplaceInline(*Key, *Entry.Value);
	}
	return Result;
}

UTexture2D* UItemBlueprintLibrary::SubstituteMessageIcon(
	UTexture2D* MessageIcon,
	const FMessageSubstitutions& MessageSubstitutions
)
{
	if (MessageSubstitutions.Icon != nullptr)
	{
		return MessageSubstitutions.Icon;
	}
	return MessageIcon;
}
