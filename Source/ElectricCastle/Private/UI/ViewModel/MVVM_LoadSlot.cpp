// Copyright Alien Shores


#include "UI/ViewModel/MVVM_LoadSlot.h"

#include "Game/Save/ElectricCastleSaveGame.h"

void UMVVM_LoadSlot::ShowEnterName()
{
	SetWidgetSwitcherIndexDelegate.Broadcast(1);
}

void UMVVM_LoadSlot::InitializeSlot()
{
	SetWidgetSwitcherIndexDelegate.Broadcast(LoadSlotStatus.GetValue());
}

void UMVVM_LoadSlot::SetPlayerName(const FString& InPlayerName)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerName, InPlayerName);
}

void UMVVM_LoadSlot::SetLoadSlotName(const FString& InLoadSlotName)
{
	UE_MVVM_SET_PROPERTY_VALUE(LoadSlotName, InLoadSlotName);
}

TEnumAsByte<ESaveSlotStatus> UMVVM_LoadSlot::GetLoadSlotStatus() const
{
	return LoadSlotStatus;
}

void UMVVM_LoadSlot::SetLoadSlotStatus(const TEnumAsByte<ESaveSlotStatus>& InLoadSlotStatus)
{
	this->LoadSlotStatus = InLoadSlotStatus;
}

int32 UMVVM_LoadSlot::GetPlayerLevel() const
{
	return PlayerLevel;
}

void UMVVM_LoadSlot::SetPlayerLevel(const int32 InPlayerLevel)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerLevel, InPlayerLevel);
}

FName UMVVM_LoadSlot::GetPlayerStartTag() const
{
	return PlayerStartTag;
}

void UMVVM_LoadSlot::SetPlayerStartTag(const FName& InPlayerStartTag)
{
	this->PlayerStartTag = InPlayerStartTag;
}

FString UMVVM_LoadSlot::GetMapAssetName() const
{
	return MapAssetName;
}

void UMVVM_LoadSlot::SetMapAssetName(const FString& InMapAssetName)
{
	this->MapAssetName = InMapAssetName;
}

void UMVVM_LoadSlot::FromSaveGame(const UElectricCastleSaveGame* SaveGame)
{
	if (SaveGame)
	{
		SetLoadSlotName(SaveGame->SaveSlotName);
		SetPlayerName(SaveGame->MetaData.PlayerName);
		SetPlayerLevel(SaveGame->MetaData.PlayerLevel);
		SetMapName(SaveGame->MetaData.MapDisplayName);
		SetLoadSlotStatus(Taken);
		SetPlayerStartTag(SaveGame->MetaData.PlayerStartTag);
	}
	InitializeSlot();
}

FString UMVVM_LoadSlot::GetMapName() const
{
	return MapName;
}

void UMVVM_LoadSlot::SetMapName(const FString& InMapName)
{
	UE_MVVM_SET_PROPERTY_VALUE(MapName, InMapName);
}
