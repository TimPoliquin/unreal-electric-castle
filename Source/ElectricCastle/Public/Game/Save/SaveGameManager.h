// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "SaveGameTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveGameManager.generated.h"

class UElectricCastleSaveGame;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAuraSaveEventSignature, const bool, bSuccess);

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FAuraSaveGameParams
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	FString DestinationMapName = FString("");
	UPROPERTY(BlueprintReadWrite)
	FName DestinationPlayerStartTag = NAME_None;
};

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API USaveGameManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static USaveGameManager* Get(const UObject* WorldContextObject);
	USaveGameManager();

	// Subsystem overrides
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// Save/Load functions
	UFUNCTION(BlueprintCallable)
	TArray<FString> GetAllSaveGameSlotNames() const;
	UFUNCTION(BlueprintCallable, Category = "Save System")
	FString AutoSave_LevelTransition(const FAuraSaveGameParams& SaveParams);
	UFUNCTION(BlueprintCallable, Category = "Save System")
	UElectricCastleSaveGame* CreateNewGame(const FString& PlayerName);
	UFUNCTION(BlueprintCallable, Category = "Save System")
	void SaveGame(const FAuraSaveGameParams& SaveParams);
	UFUNCTION(BlueprintCallable, Category = "Save System")
	void LoadGame(const FString& SlotName, int32 SlotIndex);
	UFUNCTION(BlueprintCallable, Category = "Save System")
	void LoadMostRecentGame();

	void ApplySaveGame(UElectricCastleSaveGame* LoadedData);
	void ApplySaveGame(const FString& SaveSlot, int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Save System")
	bool DoesSaveGameExist(const FString& SlotName, const int32 SlotIndex) const;

	UFUNCTION(BlueprintCallable, Category = "Save System")
	void DeleteSaveGame(const FString& SlotName = "Default");

	// Utility functions
	FString GetCurrentSaveSlotName() const;
	UElectricCastleSaveGame* LoadSaveGameData(const FString& SlotName, const int32 SlotIndex);
	int32 GetAutoSaveSlotIndex() const { return AutoSaveSlotIndex; }


	// Events
	UPROPERTY(BlueprintAssignable, Category = "Save System")
	FAuraSaveEventSignature OnSaveCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Save System")
	FAuraSaveEventSignature OnLoadCompleted;

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UElectricCastleSaveGame> SaveGameClass;
	UPROPERTY(EditDefaultsOnly)
	bool bAllowDefaultSave = false;

	UElectricCastleSaveGame* GetCurrentPrimarySaveGame() const;
	UElectricCastleSaveGame* GetCurrentAutoSaveGame() const;
	// Internal save/load logic
	void SaveMetaData(UElectricCastleSaveGame* SaveGame);
	void SaveGlobalData(UElectricCastleSaveGame* SaveGame);
	void SaveWorldData(UElectricCastleSaveGame* SaveData, FWorldSaveData& WorldSaveData);
	void SaveActorsData(UElectricCastleSaveGame* SaveData);
	void SaveActorData(AActor* Actor, FActorSaveData& ActorData);
	void SaveComponentData(UActorComponent* Component, FComponentSaveData& ComponentData);
	void SaveGameData(UElectricCastleSaveGame* CurrentSaveData);

	void LoadMetaData(const UElectricCastleSaveGame* SaveData);
	void LoadGlobalData(UElectricCastleSaveGame* SaveData);
	void LoadWorldData(const UElectricCastleSaveGame* SaveData, const FWorldSaveData& WorldSaveData);
	void LoadActorsData(const UElectricCastleSaveGame* SaveData, const FWorldSaveData& WorldSaveData);
	void LoadActor(AActor* TargetActor, const FActorSaveData& ActorData);
	AActor* LoadActorData(const FActorSaveData& ActorData);
	void LoadComponentData(UActorComponent* Component, const FComponentSaveData& ComponentData);

	UPROPERTY(EditDefaultsOnly)
	int32 AutoSaveSlotIndex = 1;

private:
	// Current save data
	FString CurrentSaveSlotName;
	bool bIsMostRecentSaveAutoSave = false;
};
