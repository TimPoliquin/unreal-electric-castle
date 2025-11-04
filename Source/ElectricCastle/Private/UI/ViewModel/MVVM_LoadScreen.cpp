// Copyright Alien Shores


#include "UI/ViewModel/MVVM_LoadScreen.h"

#include "Game/Save/SaveGameManager.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadScreen::InitializeLoadSlots()
{
	LoadSlot_0 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_0->SetLoadSlotName(TEXT("LoadSlot_0"));
	LoadSlot_0->SetSlotIndex(0);
	LoadSlot_1 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_1->SetLoadSlotName(TEXT("LoadSlot_1"));
	LoadSlot_1->SetSlotIndex(1);
	LoadSlot_2 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_2->SetLoadSlotName(TEXT("LoadSlot_2"));
	LoadSlot_2->SetSlotIndex(2);
	LoadSlots.Add(LoadSlot_0);
	LoadSlots.Add(LoadSlot_1);
	LoadSlots.Add(LoadSlot_2);
}

void UMVVM_LoadScreen::NewSlotButtonPressed(const int32 SlotIndex, const FString& EnteredName)
{
	UMVVM_LoadSlot* LoadSlot = GetLoadSlotByIndex(SlotIndex);
	FString PlayerName = EnteredName.IsEmpty()
		                     ? FString::Printf(TEXT("Player %d"), SlotIndex + 1)
		                     : EnteredName;
	USaveGameManager* SaveGameManager = USaveGameManager::Get(GetLocalPlayer());
	const UElectricCastleSaveGame* NewGame = SaveGameManager->CreateNewGame(EnteredName);
	LoadSlot->FromSaveGame(NewGame);
	LoadSlot->InitializeSlot();
	SelectSlotButtonPressed(SlotIndex);
}

void UMVVM_LoadScreen::NewGameButtonPressed(const int32 SlotIndex)
{
	GetLoadSlotByIndex(SlotIndex)->ShowEnterName();
}

void UMVVM_LoadScreen::SelectSlotButtonPressed(const int32 SlotIndex)
{
	for (const UMVVM_LoadSlot* LoadSlot : LoadSlots)
	{
		LoadSlot->EnableSelectSlotButtonDelegate.Broadcast(LoadSlot->GetSlotIndex() != SlotIndex);
	}
	if (SlotIndex >= 0)
	{
		SelectedSlot = GetLoadSlotByIndex(SlotIndex);
		SlotSelectedDelegate.Broadcast();
	}
}

void UMVVM_LoadScreen::DeleteButtonPressed()
{
	if (!IsValid(SelectedSlot))
	{
		return;
	}
	if (USaveGameManager* SaveGameManager = USaveGameManager::Get(GetLocalPlayer()))
	{
		SaveGameManager->DeleteSaveGame(SelectedSlot->GetLoadSlotName());
	}
	SelectedSlot->SetLoadSlotStatus(Vacant);
	SelectedSlot->InitializeSlot();
	ClearSelectedSlot();
}

void UMVVM_LoadScreen::ClearSelectedSlot()
{
	SelectedSlot = nullptr;
	for (const UMVVM_LoadSlot* LoadSlot : LoadSlots)
	{
		LoadSlot->EnableSelectSlotButtonDelegate.Broadcast(true);
	}
}

void UMVVM_LoadScreen::PlayButtonPressed()
{
	if (IsValid(SelectedSlot))
	{
		USaveGameManager::Get(GetLocalPlayer())->LoadGame(SelectedSlot->GetLoadSlotName(), 0);
	}
}

void UMVVM_LoadScreen::LoadData()
{
	if (USaveGameManager* SaveGameManager = USaveGameManager::Get(GetLocalPlayer()))
	{
		TArray<FString> SaveSlotNames = SaveGameManager->GetAllSaveGameSlotNames();
		for (int32 Idx = 0; Idx < SaveSlotNames.Num(); Idx++)
		{
			const FString& SaveSlotName = SaveSlotNames[Idx];
			if (LoadSlots.IsValidIndex(Idx))
			{
				LoadSlots[Idx]->FromSaveGame(SaveGameManager->LoadSaveGameData(SaveSlotName, 0));
			}
		}
	}
}

ULocalPlayer* UMVVM_LoadScreen::GetLocalPlayer() const
{
	return LocalPlayer;
}

void UMVVM_LoadScreen::SetLocalPlayer(class ULocalPlayer* InLocalPlayer)
{
	LocalPlayer = InLocalPlayer;
}
