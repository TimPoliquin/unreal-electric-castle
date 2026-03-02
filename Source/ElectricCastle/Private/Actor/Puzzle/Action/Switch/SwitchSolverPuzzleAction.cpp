// Copyright Alien Shores


#include "Actor/Puzzle/Action/Switch/SwitchSolverPuzzleAction.h"

#include "Actor/Puzzle/Manager/PuzzleManagerComponent.h"
#include "Actor/Switch/SwitchInterface.h"

bool USwitchSolverPuzzleAction::ShouldExecute_Implementation() const
{
	return Switches.Num() > 0;
}

void USwitchSolverPuzzleAction::Execute_Implementation() const
{
	for (AActor* Switch : Switches)
	{
		if (ISwitchInterface* SwitchInterface = Cast<ISwitchInterface>(Switch))
		{
			SwitchInterface->GetOnSwitchActivatedDelegate().AddUniqueDynamic(this, &USwitchSolverPuzzleAction::HandleButtonActivated);
		}
	}
}

void USwitchSolverPuzzleAction::HandleButtonActivated_Implementation(const FOnSwitchStatusChangedPayload& Payload)
{
	bool bAllActivated = true;
	bool PayloadSwitchFound = false;
	bool bTriggerReset = false;
	for (AActor* Switch : Switches)
	{
		if (Payload.Switch == Switch)
		{
			PayloadSwitchFound = true;
		}
		if (!ISwitchInterface::IsSwitchActive(Switch))
		{
			bAllActivated = false;
			if (bOrdered && !PayloadSwitchFound)
			{
				bTriggerReset = true;
				break;
			}
		}
	}
	if (bTriggerReset)
	{
		ResetSwitches();
	}
	if (bAllActivated)
	{
		MarkPuzzleSolved();
	}
}


void USwitchSolverPuzzleAction::ResetSwitches_Implementation()
{
	for (AActor* Switch : Switches)
	{
		ISwitchInterface::ResetSwitch(Switch);
	}
}

void USwitchSolverPuzzleAction::MarkPuzzleSolved_Implementation()
{
	if (UPuzzleManagerComponent* LocalPuzzleManager = GetPuzzleManager())
	{
		LocalPuzzleManager->SolvePuzzle();
	}
}
