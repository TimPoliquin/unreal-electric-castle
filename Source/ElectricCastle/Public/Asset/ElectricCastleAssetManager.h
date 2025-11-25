// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "ElectricCastleAssetManager.generated.h"

/**
 * Configured in DefaultEngine.ini
 */
UCLASS()
class ELECTRICCASTLE_API UElectricCastleAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	static UElectricCastleAssetManager& Get();
	static TSharedPtr<FStreamableHandle> LoadPlayerFormPrimaryAssets(const TArray<FPrimaryAssetId>& AssetIds, const FStreamableDelegate& OnLoaded);

protected:
	virtual void StartInitialLoading() override;

private:
	static TSharedPtr<FStreamableHandle> LoadPrimaryAssetsById(const TArray<FPrimaryAssetId>& AssetIds, const TArray<FName>& BundleNames, const FStreamableDelegate& OnLoaded);
	static void DebugAssetRegistry();
	static void ForceAssetManagerRescan();
	static void TestDirectAssetLoad();
	static void DebugAssetManagerConfig();
};
