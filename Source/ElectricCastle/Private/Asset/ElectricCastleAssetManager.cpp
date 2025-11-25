// Copyright Alien Shores


#include "Asset/ElectricCastleAssetManager.h"

#include "Tags/ElectricCastleGameplayTags.h"
#include "AbilitySystemGlobals.h"
#include "Player/Form/PlayerFormPrimaryAsset.h"
#include "Engine/AssetManager.h"

UElectricCastleAssetManager& UElectricCastleAssetManager::Get()
{
	return *Cast<UElectricCastleAssetManager>(GEngine->AssetManager);
}

TSharedPtr<FStreamableHandle> UElectricCastleAssetManager::LoadPlayerFormPrimaryAssets(const TArray<FPrimaryAssetId>& AssetIds, const FStreamableDelegate& OnLoaded)
{
	UAssetManager::Get().ScanPathsForPrimaryAssets(
		FPrimaryAssetType("PlayerForm"),
		{TEXT("/Game/Blueprints/Player/Data")},
		UPlayerFormPrimaryAsset::StaticClass(),
		false
	);
	return LoadPrimaryAssetsById(AssetIds, {"FormsBundle"}, OnLoaded);
}

void UElectricCastleAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	FElectricCastleGameplayTags::InitializeNativeGameplayTags();
	// DEVNOTE: Required to use TargetData (TargetDataUnderMouse)
	UAbilitySystemGlobals::Get().InitGlobalData();
}

TSharedPtr<FStreamableHandle> UElectricCastleAssetManager::LoadPrimaryAssetsById(const TArray<FPrimaryAssetId>& AssetIds, const TArray<FName>& BundleNames, const FStreamableDelegate& OnLoaded)
{
	UE_LOG(LogTemp, Log, TEXT("[ElectricCastleAssetManager] Started preloading %d assets..."), AssetIds.Num());
	return UAssetManager::Get().LoadPrimaryAssets(
		AssetIds,
		BundleNames,
		OnLoaded,
		FStreamableManager::AsyncLoadHighPriority
	);
}
