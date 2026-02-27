// Copyright Alien Shores


#include "Actor/Puzzle/Action/Lock/UnlockPuzzleAction.h"

#include "Actor/Lock/LockInterface.h"

bool UUnlockPuzzleAction::ShouldExecute_Implementation() const
{
	return ToUnlock.Num() > 0;
}

void UUnlockPuzzleAction::Execute_Implementation() const
{
	for (AActor* Actor : ToUnlock)
	{
		ILockInterface::Unlock(Actor);
	}
}
