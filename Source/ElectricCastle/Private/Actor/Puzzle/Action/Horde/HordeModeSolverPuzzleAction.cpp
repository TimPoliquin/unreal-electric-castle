// Copyright Alien Shores


#include "Actor/Puzzle/Action/Horde/HordeModeSolverPuzzleAction.h"

#include "Actor/Trigger/TriggerableInterface.h"
#include "Actor/Horde/HordeModeActorInterface.h"
#include "Actor/Horde/HordeModeComponent.h"
#include "Actor/Puzzle/Manager/PuzzleManagerComponent.h"

bool UHordeModeSolverPuzzleAction::ShouldExecute_Implementation() const
{
	return IsValid(HordeModeActor) && HordeModeActor->Implements<UHordeModeActorInterface>();
}

void UHordeModeSolverPuzzleAction::Execute_Implementation() const
{
	if (UHordeModeComponent* HordeModeComponent = IHordeModeActorInterface::GetHordeModeComponent(HordeModeActor))
	{
		HordeModeComponent->OnHordeModeRoundComplete.AddUniqueDynamic(this, &UHordeModeSolverPuzzleAction::OnHordeModeRoundComplete);
	}
	if (!bStartHordeOnExecute)
	{
		return;
	}
	// if the horde mode actor is a trigger, use the trigger interface to activate.
	// otherwise, just start the round.
	if (ITriggerableInterface::IsTrigger(HordeModeActor))
	{
		ITriggerableInterface::Trigger(HordeModeActor, true);
	}
	else if (UHordeModeComponent* HordeModeComponent = IHordeModeActorInterface::GetHordeModeComponent(HordeModeActor))
	{
		HordeModeComponent->StartRound();
	}
}

void UHordeModeSolverPuzzleAction::OnHordeModeRoundComplete(const FOnHordeModeRoundCompletePayload& Payload)
{
	if (Payload.HasNextRound)
	{
		return;
	}
	if (UPuzzleManagerComponent* LocalPuzzleManager = GetPuzzleManager())
	{
		LocalPuzzleManager->SolvePuzzle();
	}
}
