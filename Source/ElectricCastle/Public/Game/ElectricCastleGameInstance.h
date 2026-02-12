// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ElectricCastleGameInstance.generated.h"

class UMusicManagerSubsystem;
class UPlayerManager;
class USaveGameManager;
class UElectricCastleAIDirectorGameInstanceSubsystem;
class UElectricCastleGameDataSubsystem;
class UElectricCastleLevelManager;
/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UElectricCastleGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UElectricCastleGameInstance();
	static UElectricCastleGameInstance* Get(const UObject* WorldContextObject);

protected:
	UPROPERTY(EditDefaultsOnly, Category="Subsystem")
	TSubclassOf<UElectricCastleAIDirectorGameInstanceSubsystem> AIDirectorSubsystem;
	UPROPERTY(EditDefaultsOnly, Category = "Subsystem")
	TSubclassOf<UElectricCastleGameDataSubsystem> CharacterSubsystem;
	UPROPERTY(EditDefaultsOnly, Category = "Subsystem")
	TSubclassOf<UElectricCastleLevelManager> LevelSubsystem;
	UPROPERTY(EditDefaultsOnly, Category="Subsystem")
	TSubclassOf<UMusicManagerSubsystem> MusicManager;
	UPROPERTY(EditDefaultsOnly, Category = "Subsystem")
	TSubclassOf<UPlayerManager> PlayerManager;
	UPROPERTY(EditDefaultsOnly, Category = "Subsystem")
	TSubclassOf<USaveGameManager> SaveGameSubsystem;
};
