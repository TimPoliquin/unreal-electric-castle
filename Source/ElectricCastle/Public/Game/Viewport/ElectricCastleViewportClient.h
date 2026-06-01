// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "CommonGameViewportClient.h"
#include "Engine/GameViewportClient.h"
#include "ElectricCastleViewportClient.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UElectricCastleViewportClient : public UCommonGameViewportClient
{
	GENERATED_BODY()
	friend class FSignificanceManagerModule;

public:
	virtual void Tick(float DeltaTime) override;
};
