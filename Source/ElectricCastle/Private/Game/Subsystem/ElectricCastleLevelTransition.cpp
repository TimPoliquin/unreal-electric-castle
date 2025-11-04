// Copyright Alien Shores


#include "Game/Subsystem/ElectricCastleLevelTransition.h"

#include "Game/Save/SaveGameManager.h"
#include "Game/Subsystem/ElectricCastleLevelManager.h"

void UElectricCastleLevelTransition::Initialize(const FAuraLevelTransitionParams& Params)
{
	DestinationPlayerStartTag = Params.PlayerStartTag;
	bShouldLoad = Params.ShouldLoad();
	SaveSlot = Params.SaveSlot;
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddWeakLambda(this, [this](UWorld* World)
	{
		if (bShouldLoad)
		{
			USaveGameManager::Get(World)->ApplySaveGame(SaveSlot, SlotIndex);
		}
		OnComplete.Broadcast(World);
	});
}

FName UElectricCastleLevelTransition::GetDestinationPlayerStartTag() const
{
	return DestinationPlayerStartTag;
}
