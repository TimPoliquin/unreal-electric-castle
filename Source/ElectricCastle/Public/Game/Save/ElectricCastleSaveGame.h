// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "SaveGameTypes.h"
#include "GameFramework/SaveGame.h"
#include "ElectricCastleSaveGame.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API UElectricCastleSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	static const FString DEFAULT_SAVE_SLOT_NAME;

	UElectricCastleSaveGame();

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	int32 SaveVersion;

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	FString SaveSlotName;

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	FMetaSaveData MetaData;

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	FGlobalSaveData GlobalData;


	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	TArray<FWorldSaveData> WorldsData;

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	bool bIsAutoSave = false;

	// Utility functions
	UFUNCTION(BlueprintCallable, Category = "Save Data")
	bool FindWorldSaveData(FString WorldName, FWorldSaveData& WorldSaveData);
	UFUNCTION(BlueprintCallable, Category = "Save Data")
	void GetOrCreateWorldSaveData(FString WorldName, FWorldSaveData& WorldSaveData);
	UFUNCTION(BlueprintCallable, Category = "Save Data")
	void SetActorsData(const FString& WorldPathName, TArray<FActorSaveData> ActorsSaveData);
	UFUNCTION(BlueprintCallable, Category = "Save Data")
	void ClearAllData();

private:
	static constexpr int32 CURRENT_SAVE_VERSION = 1;
};
