// Copyright Alien Shores


#include "Actor/Puzzle/Manager/PuzzleManagerActorInterface.h"

#include "Actor/Puzzle/Manager/PuzzleManagerComponent.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"


// Add default functionality here for any IPuzzleManagerActorInterface functions that are not pure virtual.
bool IPuzzleManagerActorInterface::IsPuzzleManagerActor(const UObject* Object)
{
	return IsValid(Object) && Object->Implements<UPuzzleManagerActorInterface>();
}

UPuzzleManagerComponent* IPuzzleManagerActorInterface::GetPuzzleManagerComponent(const UObject* Object)
{
	if (IsPuzzleManagerActor(Object))
	{
		return Execute_GetPuzzleManagerComponent(Object);
	}
	return nullptr;
}

void IPuzzleManagerActorInterface::ActivatePuzzle(const UObject* PuzzleActor)
{
	if (UPuzzleManagerComponent* PuzzleManagerComponent = GetPuzzleManagerComponent(PuzzleActor))
	{
		PuzzleManagerComponent->ActivatePuzzle();
	}
	else
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("PuzzleManagerComponent not found for PuzzleActor %s"), PuzzleActor ? *PuzzleActor->GetName() : *FString("NULLPTR"))
	}
}

void IPuzzleManagerActorInterface::SolvePuzzle(const UObject* PuzzleActor)
{
	if (UPuzzleManagerComponent* PuzzleManagerComponent = GetPuzzleManagerComponent(PuzzleActor))
	{
		PuzzleManagerComponent->SolvePuzzle();
	}
	else
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("PuzzleManagerComponent not found for PuzzleActor %s"), PuzzleActor ? *PuzzleActor->GetName() : *FString("NULLPTR"))
	}
}

void IPuzzleManagerActorInterface::HandlePuzzleActivated(UObject* PuzzleActor)
{
	if (IsPuzzleManagerActor(PuzzleActor))
	{
		Execute_HandlePuzzleActivated(PuzzleActor);
	}
}

void IPuzzleManagerActorInterface::HandlePuzzleSolved(UObject* PuzzleActor)
{
	if (IsPuzzleManagerActor(PuzzleActor))
	{
		Execute_HandlePuzzleSolved(PuzzleActor);
	}
}
