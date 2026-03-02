// Copyright Alien Shores


#include "Actor/Puzzle/Action/Music/MusicEndOverridePuzzleAction.h"

#include "Game/Subsystem/MusicManagerSubsystem.h"

void UMusicEndOverridePuzzleAction::Execute_Implementation() const
{
	if (UMusicManagerSubsystem* MusicManagerSubsystem = UMusicManagerSubsystem::Get(GetOwner()))
	{
		MusicManagerSubsystem->EndOverrideMusic();
	}
}
