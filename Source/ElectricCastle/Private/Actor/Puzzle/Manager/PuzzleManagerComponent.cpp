// Copyright Alien Shores


#include "Actor/Puzzle/Manager/PuzzleManagerComponent.h"

#include "Actor/Puzzle/Manager/PuzzleManagerActorInterface.h"
#include "Actor/Puzzle/Action/PuzzleAction.h"
#include "Actor/Puzzle/Action/Trigger/TriggeringPuzzleAction.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Net/UnrealNetwork.h"


// Sets default values
UPuzzleManagerComponent::UPuzzleManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPuzzleManagerComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UPuzzleManagerComponent, PuzzleState);
}

// Called when the game starts or when spawned
void UPuzzleManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	for (UPuzzleAction* PuzzleAction : OnActivatePuzzleActions)
	{
		PuzzleAction->Initialize(GetOwner(), this);
	}
	for (UPuzzleAction* PuzzleAction : OnSolvePuzzleActions)
	{
		PuzzleAction->Initialize(GetOwner(), this);
	}
}

void UPuzzleManagerComponent::ActivatePuzzle_Implementation()
{
	if (PuzzleState >= EPuzzleManagerState::Active)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] Puzzle already activated!"), *GetOwner()->GetName(), *GetName());
		return;
	}
	PuzzleState = EPuzzleManagerState::Active;
	for (const UPuzzleAction* PuzzleAction : OnActivatePuzzleActions)
	{
		if (IsValid(PuzzleAction) && PuzzleAction->ShouldExecute())
		{
			PuzzleAction->Execute();
		}
	}
	IPuzzleManagerActorInterface::HandlePuzzleActivated(GetOwner());
}

void UPuzzleManagerComponent::SolvePuzzle_Implementation()
{
	if (PuzzleState >= EPuzzleManagerState::Solved)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] Puzzle already solved!!"), *GetOwner()->GetName(), *GetName());
		return;
	}
	PuzzleState = EPuzzleManagerState::Solved;
	for (const UPuzzleAction* PuzzleAction : OnSolvePuzzleActions)
	{
		if (IsValid(PuzzleAction) && PuzzleAction->ShouldExecute())
		{
			PuzzleAction->Execute();
		}
	}
	IPuzzleManagerActorInterface::HandlePuzzleSolved(GetOwner());
}
