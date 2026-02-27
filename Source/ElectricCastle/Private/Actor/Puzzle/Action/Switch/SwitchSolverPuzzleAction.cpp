// Copyright Alien Shores


#include "Actor/Puzzle/Action/Switch/SwitchSolverPuzzleAction.h"

#include "Actor/Puzzle/Manager/PuzzleManagerComponent.h"
#include "Actor/Switch/SwitchInterface.h"

bool USwitchSolverPuzzleAction::ShouldExecute_Implementation() const
{
	return ISwitchInterface::IsSwitchActor(Switch);
}

void USwitchSolverPuzzleAction::Execute_Implementation() const
{
	if (ISwitchInterface* SwitchInterface = Cast<ISwitchInterface>(Switch))
	{
		SwitchInterface->GetOnSwitchActivatedDelegate().AddUniqueDynamic(this, &USwitchSolverPuzzleAction::HandleButtonActivated);
	}
}

void USwitchSolverPuzzleAction::HandleButtonActivated_Implementation(const FOnSwitchStatusChangedPayload& Payload)
{
	if (UPuzzleManagerComponent* LocalPuzzleManager = GetPuzzleManager())
	{
		LocalPuzzleManager->SolvePuzzle();
	}
}
