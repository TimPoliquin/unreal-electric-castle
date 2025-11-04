// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ElectricCastleGameModeBase.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API AElectricCastleGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	static AElectricCastleGameModeBase* GetElectricCastleGameMode(const UObject* WorldContextObject);

	/** Do not put UPROPERTY attributes here! They will not be available in multiplayer modes! **/
};
