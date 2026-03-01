// Copyright Alien Shores


#include "Actor/Puzzle/Action/Lock/LockPuzzleAction.h"

#include "Actor/Lock/LockInterface.h"

bool ULockPuzzleAction::ShouldExecute_Implementation() const
{
	return ToLock.Num() > 0;
}

void ULockPuzzleAction::Execute_Implementation() const
{
	for (AActor* Actor : ToLock)
	{
		LockActor(Actor);
	}
}

void ULockPuzzleAction::LockActor_Implementation(AActor* Actor) const
{
	ILockInterface::Lock(Actor);
}
