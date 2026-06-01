// Copyright Alien Shores


#include "Actor/Puzzle/Action/Trigger/TriggeringPuzzleAction.h"

#include "Actor/Trigger/TriggerableInterface.h"

void UTriggeringPuzzleAction::Execute_Implementation() const
{
	for (AActor* TriggerActor : TriggerActors)
	{
		ITriggerableInterface::Trigger(TriggerActor, TriggerMode == EPuzzleActionTriggerMode::Activate);
	}
}

UTriggeringPuzzleAction* UTriggeringPuzzleAction::Create(UObject* Outer, const FName Name, const EPuzzleActionTriggerMode InTriggerMode)
{
	UTriggeringPuzzleAction* NewAction = NewObject<UTriggeringPuzzleAction>(Outer, Name);
	NewAction->TriggerMode = InTriggerMode;
	return NewAction;
}
