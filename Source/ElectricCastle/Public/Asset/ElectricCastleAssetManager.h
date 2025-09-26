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

protected:
	virtual void StartInitialLoading() override;
};
