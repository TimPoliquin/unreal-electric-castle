// Copyright Alien Shores


#include "Game/Subsystem/ElectricCastleGameDataSubsystem.h"

#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Asset/ElectricCastleAssetManager.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Engine/AssetManager.h"
#include "Item/Data/ItemDefinitions.h"
#include "Kismet/GameplayStatics.h"
#include "Player/Form/PlayerFormPrimaryAsset.h"

void UElectricCastleGameDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	InitializeItemDefinitions();
	if (TArray<FPrimaryAssetId> FormAssetIds; ShouldPreloadForms(FormAssetIds))
	{
		UElectricCastleAssetManager::LoadPlayerFormPrimaryAssets(FormAssetIds, FStreamableDelegate::CreateUObject(this, &UElectricCastleGameDataSubsystem::OnFormDataInitialized));
	}
	else
	{
		bIsGameDataLoaded = true;
		OnGameDataLoaded.Broadcast();
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

UPlayerFormPrimaryAsset* UElectricCastleGameDataSubsystem::GetPlayerFormConfigById(const EPlayerForm& FormId) const
{
	const TSoftObjectPtr<UPlayerFormPrimaryAsset>* FormAsset = PlayerFormPrimaryAssets.FindByPredicate([FormId](const TSoftObjectPtr<UPlayerFormPrimaryAsset> Config)
	{
		return Config.LoadSynchronous()->FormId == FormId;
	});
	if (FormAsset && FormAsset->IsValid())
	{
		return FormAsset->Get();
	}
	UE_LOG(LogElectricCastle, Error, TEXT("[%s] Requested form id %s is not valid"), *GetName(), *UEnum::GetValueAsString(FormId));
	return nullptr;
}

UPlayerFormPrimaryAsset* UElectricCastleGameDataSubsystem::GetPlayerFormConfigById(const int32 FormId) const
{
	if (FormId <= 0 || FormId >= static_cast<int32>(EPlayerForm::Count))
	{
		UE_LOG(
			LogElectricCastle,
			Warning,
			TEXT("[%s] Requested form id is outside of known range. %d"),
			*GetName(),
			FormId
		)
		return nullptr;
	}
	return GetPlayerFormConfigById(static_cast<EPlayerForm>(FormId));
}

UPlayerFormPrimaryAsset* UElectricCastleGameDataSubsystem::GetPlayerFormConfigByTag(const FGameplayTag& FormTag) const
{
	const TSoftObjectPtr<UPlayerFormPrimaryAsset>* FormAsset = PlayerFormPrimaryAssets.FindByPredicate([FormTag](const TSoftObjectPtr<UPlayerFormPrimaryAsset> Config)
	{
		return Config.LoadSynchronous()->FormTag.MatchesTagExact(FormTag);
	});
	if (FormAsset && FormAsset->IsValid())
	{
		return FormAsset->Get();
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

void UElectricCastleGameDataSubsystem::OnFormDataInitialized()
{
	const UAssetManager& AssetManager = UAssetManager::Get();

	// Get all loaded form assets
	TArray<FPrimaryAssetId> FormAssetIds;
	AssetManager.GetPrimaryAssetIdList(FPrimaryAssetType("PlayerForm"), FormAssetIds);

	PlayerFormPrimaryAssets.Empty();
	for (const FPrimaryAssetId& AssetId : FormAssetIds)
	{
		UPlayerFormPrimaryAsset* FormAsset = Cast<UPlayerFormPrimaryAsset>(
			AssetManager.GetPrimaryAssetObject(AssetId)
		);

		if (FormAsset)
		{
			PlayerFormPrimaryAssets.Add(FormAsset);

			// Force initialization of resources
			FormAsset->PostLoad();

			UE_LOG(LogTemp, Log, TEXT("[%s] Loaded form: %s"), *GetName(), *FormAsset->FormName);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[%s] All player forms preloaded! Total: %d"), *GetName(), PlayerFormPrimaryAssets.Num());
	bIsGameDataLoaded = true;
	OnGameDataLoaded.Broadcast();
}

bool UElectricCastleGameDataSubsystem::ShouldPreloadForms(TArray<FPrimaryAssetId>& OutAssetIds) const
{
	if (!bPreloadForms || PlayerFormPrimaryAssets.IsEmpty())
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Preloading forms is disabled or no player forms have been configured yet."), *GetName());
		return false;
	}
	for (const TSoftObjectPtr<UPlayerFormPrimaryAsset>& Asset : PlayerFormPrimaryAssets)
	{
		if (!Asset.IsNull())
		{
			FString AssetName = Asset.GetAssetName();
			FPrimaryAssetId AssetId("PlayerForm", FName(*AssetName));
			OutAssetIds.Add(AssetId);
		}
	}
	return OutAssetIds.Num() > 0;
}
