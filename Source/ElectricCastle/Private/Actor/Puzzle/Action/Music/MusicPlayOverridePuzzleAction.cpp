// Copyright Alien Shores


#include "Actor/Puzzle/Action/Music/MusicPlayOverridePuzzleAction.h"

#include "Game/Subsystem/MusicManagerSubsystem.h"

bool UMusicPlayOverridePuzzleAction::ShouldExecute_Implementation() const
{
	return IsValid(MusicOverride);
}

void UMusicPlayOverridePuzzleAction::Execute_Implementation() const
{
	if (UMusicManagerSubsystem* MusicManagerSubsystem = UMusicManagerSubsystem::Get(GetOwner()))
	{
		MusicManagerSubsystem->StartOverrideMusic(MusicOverride);
	}
}
