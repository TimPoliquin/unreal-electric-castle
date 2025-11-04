// Copyright Alien Shores


#include "Asset/ElectricCastleAssetManager.h"

#include "Tags/ElectricCastleGameplayTags.h"
#include "AbilitySystemGlobals.h"

UElectricCastleAssetManager& UElectricCastleAssetManager::Get()
{
	return *Cast<UElectricCastleAssetManager>(GEngine->AssetManager);
}

void UElectricCastleAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	FElectricCastleGameplayTags::InitializeNativeGameplayTags();
	// DEVNOTE: Required to use TargetData (TargetDataUnderMouse)
	UAbilitySystemGlobals::Get().InitGlobalData();
}
