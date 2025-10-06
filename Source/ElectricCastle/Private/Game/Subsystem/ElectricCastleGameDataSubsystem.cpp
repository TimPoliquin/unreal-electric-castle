// Copyright Alien Shores


#include "Game/Subsystem/ElectricCastleGameDataSubsystem.h"

#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Item/Data/ItemDefinitions.h"
#include "Kismet/GameplayStatics.h"
#include "Player/Form/PlayerFormConfig.h"

void UElectricCastleGameDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	InitializeItemDefinitions();
	if (PlayerFormConfig)
	{
		PlayerFormConfig->Initialize();
	}
}

UElectricCastleGameDataSubsystem* UElectricCastleGameDataSubsystem::Get(const UObject* WorldContextObject)
{
	if (const UGameInstance* GameInstance = IsValid(WorldContextObject)
		                                        ? UGameplayStatics::GetGameInstance(WorldContextObject)
		                                        : nullptr)
	{
		return GameInstance->GetSubsystem<UElectricCastleGameDataSubsystem>();
	}
	return nullptr;
}

FItemDefinition UElectricCastleGameDataSubsystem::FindItemDefinitionByItemTag(const FGameplayTag& ItemTag)
{
	if (ItemDefinitions.Contains(ItemTag))
	{
		return ItemDefinitions[ItemTag];
	}
	return FItemDefinition();
}

FGameplayTag UElectricCastleGameDataSubsystem::GetDefaultItemPickupMessageTag() const
{
	return DefaultItemPickupMessageTag;
}

FGameplayTag UElectricCastleGameDataSubsystem::GetDefaultItemUseMessageTag() const
{
	return DefaultItemUsedMessageTag;
}


float UElectricCastleGameDataSubsystem::GetXPToNextLevelPercentage(const float XP) const
{
	checkf(LevelUpInfo, TEXT("LevelUpInfo not set on UElectricCastleGameDataSubsystem - this must be set in the Blueprint"))
	return LevelUpInfo->GetLevelProgressPercentage(XP);
}

int32 UElectricCastleGameDataSubsystem::FindLevelByXP(const int32 InXP) const
{
	checkf(LevelUpInfo, TEXT("LevelUpInfo not set on UElectricCastleGameDataSubsystem - this must be set in the Blueprint"))
	return LevelUpInfo->FindLevelByXP(InXP);
}

FLevelUpRewards UElectricCastleGameDataSubsystem::GetLevelUpRewards(int32 CurrentLevel) const
{
	checkf(LevelUpInfo, TEXT("LevelUpInfo not set on UElectricCastleGameDataSubsystem - this must be set in the Blueprint"))
	return LevelUpInfo->GetRewardsByLevel(CurrentLevel);
}

void UElectricCastleGameDataSubsystem::InitializeItemDefinitions()
{
	if (!ItemDefinitions.IsEmpty())
	{
		return;
	}
	for (const UItemDefinitions* ItemDefinitionSet : ItemInfos)
	{
		ItemDefinitionSet->AddToMap(ItemDefinitions);
	}
}
