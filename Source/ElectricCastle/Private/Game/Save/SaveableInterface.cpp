// Copyright Alien Shores


#include "Game/Save/SaveableInterface.h"

#include "Serialization/ObjectAndNameAsStringProxyArchive.h"


TArray<uint8> ISaveableInterface::SaveData_Implementation()
{
	TArray<uint8> Data;
	return Data;
}

bool ISaveableInterface::LoadData_Implementation(const TArray<uint8>& Data)
{
	if (AActor* ThisObject = Cast<AActor>(this))
	{
		FMemoryReader MemoryReader(Data);
		FObjectAndNameAsStringProxyArchive Archive(MemoryReader, true);
		Archive.ArIsSaveGame = true;
		ThisObject->Serialize(Archive);
		return true;
	}

	return false;
}

bool ISaveableInterface::ShouldSave_Implementation() const
{
	if (const UObject* ThisObject = Cast<UObject>(this))
	{
		return IsValid(ThisObject);
	}
	return false;
}

bool ISaveableInterface::ShouldAutoSpawn_Implementation() const
{
	return false;
}

bool ISaveableInterface::ShouldLoadTransform_Implementation() const
{
	return false;
}
