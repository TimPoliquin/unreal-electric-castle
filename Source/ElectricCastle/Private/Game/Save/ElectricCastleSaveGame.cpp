// Copyright Alien Shores


#include "Game/Save/ElectricCastleSaveGame.h"

#include "ElectricCastle/ElectricCastleLogChannels.h"

const FString UElectricCastleSaveGame::DEFAULT_SAVE_SLOT_NAME = FString("Default");

UElectricCastleSaveGame::UElectricCastleSaveGame()
{
	SaveVersion = CURRENT_SAVE_VERSION;
	SaveSlotName = DEFAULT_SAVE_SLOT_NAME;
}

bool UElectricCastleSaveGame::FindWorldSaveData(const FString WorldName, FWorldSaveData& WorldSaveData)
{
	for (const FWorldSaveData& CurrentWorldSaveData : WorldsData)
	{
		if (CurrentWorldSaveData.WorldName == WorldName)
		{
			WorldSaveData = CurrentWorldSaveData;
			return true;
		}
	}
	return false;
}

void UElectricCastleSaveGame::GetOrCreateWorldSaveData(const FString WorldName, FWorldSaveData& WorldSaveData)
{
	if (!FindWorldSaveData(WorldName, WorldSaveData))
	{
		WorldSaveData.WorldName = WorldName;
		WorldsData.Add(WorldSaveData);
	}
}

void UElectricCastleSaveGame::SetActorsData(const FString& WorldPathName, TArray<FActorSaveData> ActorsSaveData)
{
	for (FWorldSaveData& WorldSaveData : WorldsData)
	{
		if (WorldSaveData.WorldName == WorldPathName)
		{
			WorldSaveData.ActorsData = ActorsSaveData;
			return;
		}
	}
	UE_LOG(LogElectricCastle, Warning, TEXT("[%s] No world save data found for world: %s"), *GetName(), *WorldPathName);
}

void UElectricCastleSaveGame::ClearAllData()
{
	MetaData = FMetaSaveData();
	GlobalData = FGlobalSaveData();
	WorldsData.Empty();
	bIsAutoSave = false;
}
