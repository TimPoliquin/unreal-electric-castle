// Copyright Alien Shores


#include "Actor/Puzzle/Action/Cinematic/CinematicSolverPuzzleAction.h"

#include "Actor/Cinematic/CinematicPlaybackActorInterface.h"
#include "Actor/Puzzle/Manager/PuzzleManagerComponent.h"

bool UCinematicSolverPuzzleAction::ShouldExecute_Implementation() const
{
	return IsValid(CinematicPlaybackActor) && CinematicPlaybackActor->Implements<UCinematicPlaybackActorInterface>();
}

void UCinematicSolverPuzzleAction::Execute_Implementation() const
{
	if (ICinematicPlaybackActorInterface* CinematicPlaybackActorInterface = Cast<ICinematicPlaybackActorInterface>(CinematicPlaybackActor))
	{
		CinematicPlaybackActorInterface->GetOnPlaybackStartDelegate().AddUniqueDynamic(this, &UCinematicSolverPuzzleAction::HandlePlaybackStart);
		CinematicPlaybackActorInterface->GetOnPlaybackEndDelegate().AddUniqueDynamic(this, &UCinematicSolverPuzzleAction::HandlePlaybackEnd);
	}
	if (bStartPlaybackOnExecute)
	{
		ICinematicPlaybackActorInterface::Start(CinematicPlaybackActor);
	}
}

void UCinematicSolverPuzzleAction::HandlePlaybackStart_Implementation()
{
	// nothing to do here
}

void UCinematicSolverPuzzleAction::HandlePlaybackEnd_Implementation()
{
	if (UPuzzleManagerComponent* LocalPuzzleManager = GetPuzzleManager())
	{
		LocalPuzzleManager->SolvePuzzle();
	}
}
