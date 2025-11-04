// Copyright Alien Shores


#include "Game/Save/SaveGameBlueprintFunctionLibrary.h"

void USaveGameBlueprintFunctionLibrary::AddActorData(const FActorSaveData& ActorData, FWorldSaveData& WorldSaveData)
{
	// Check if actor data already exists and replace it
	for (int32 i = 0; i < WorldSaveData.ActorsData.Num(); i++)
	{
		if (WorldSaveData.ActorsData[i].ActorName == ActorData.ActorName)
		{
			WorldSaveData.ActorsData[i] = ActorData;
			return;
		}
	}
	// Add new actor data if not found
	WorldSaveData.ActorsData.Add(ActorData);
}

bool USaveGameBlueprintFunctionLibrary::RemoveActorData(const FString& ActorName, FWorldSaveData& WorldSaveData)
{
	for (int32 i = 0; i < WorldSaveData.ActorsData.Num(); i++)
	{
		if (WorldSaveData.ActorsData[i].ActorName == ActorName)
		{
			WorldSaveData.ActorsData.RemoveAt(i);
			return true;
		}
	}
	return false;
}

bool USaveGameBlueprintFunctionLibrary::FindActorData(const FString& ActorName, const FWorldSaveData& WorldSaveData, FActorSaveData& ActorSaveData)
{
	for (const FActorSaveData& ActorData : WorldSaveData.ActorsData)
	{
		if (ActorData.ActorName == ActorName)
		{
			ActorSaveData = ActorData;
			return true;
		}
	}
	return false;
}

int32 USaveGameBlueprintFunctionLibrary::GetActorCount(FWorldSaveData& WorldSaveData)
{
	return WorldSaveData.ActorsData.Num();
}

FString USaveGameBlueprintFunctionLibrary::GenerateSaveID(const UObject* Object)
{
	if (!Object)
	{
		return FString("");
	}
	return FString::Printf(TEXT("%s_%s"), *Object->GetClass()->GetName(), *FGuid::NewGuid().ToString());
}
