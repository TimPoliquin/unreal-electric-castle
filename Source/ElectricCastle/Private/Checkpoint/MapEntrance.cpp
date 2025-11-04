// Copyright Alien Shores


#include "Checkpoint/MapEntrance.h"

#include "Game/Save/SaveGameManager.h"
#include "Game/Subsystem/ElectricCastleLevelManager.h"
#include "Interaction/PlayerInterface.h"

AMapEntrance::AMapEntrance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	bDisableAfterActivation = false;
}


void AMapEntrance::BeginPlay()
{
	Super::BeginPlay();
}

void AMapEntrance::OnSphereOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (IsValid(OtherActor) && OtherActor->Implements<UPlayerInterface>())
	{
		bHasBeenActivated = true;
		if (UElectricCastleLevelManager* LevelManager = UElectricCastleLevelManager::Get(this))
		{
			const USaveGameManager* SaveGameManager = USaveGameManager::Get(this);
			FAuraLevelTransitionParams Params;
			Params.SaveSlot = SaveGameManager->GetCurrentSaveSlotName();
			Params.SlotIndex = SaveGameManager->GetAutoSaveSlotIndex();
			Params.bShouldSave = true;
			Params.bShouldLoad = true;
			Params.MapAssetName = DestinationMap.GetAssetName();
			Params.PlayerStartTag = DestinationPlayerStartTag;
			LevelManager->TransitionLevel(Params);
		}
	}
}
