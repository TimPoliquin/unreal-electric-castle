// Copyright Alien Shores


#include "Cinematic/Context/CinematicContext.h"

#include "LevelSequencePlayer.h"
#include "Cinematic/Blueprint/AsyncPlayLevelSequence.h"

ULevelSequencePlayer* UCinematicContext::GetLevelSequencePlayer() const
{
	return LevelSequencePlayer.Get();
}

ULevelSequence* UCinematicContext::GetLevelSequence() const
{
	return LevelSequence.Get();
}

FGameplayTagContainer UCinematicContext::GetCinematicTagContainer() const
{
	return CinematicTags;
}

void UCinematicContext::RestoreAll()
{
	RestoreStack.RestoreAll();
}

void UCinematicContext::SetLevelSequencePlayer(ULevelSequencePlayer* InPlayer)
{
	LevelSequencePlayer = InPlayer;
	if (!IsValid(InPlayer))
	{
		return;
	}
	InPlayer->OnPlay.AddDynamic(this, &UCinematicContext::HandleOnPlay);
	InPlayer->OnFinished.AddDynamic(this, &UCinematicContext::HandleOnFinished);
	InPlayer->OnStop.AddDynamic(this, &UCinematicContext::HandleOnFinished);
}

void UCinematicContext::SetLevelSequence(ULevelSequence* InSequence)
{
	LevelSequence = InSequence;
}

void UCinematicContext::SetCinematicTags(const FGameplayTagContainer& InCinematicTags)
{
	CinematicTags = InCinematicTags;
}

void UCinematicContext::AddRestoreFunction(const TFunction<void()>& InRestoreFunc)
{
	RestoreStack.Add(InRestoreFunc);
}

void UCinematicContext::HandleOnPlay()
{
	OnCinematicBegin.Broadcast(FCinematicContextEventPayload(ECinematicLifeCycle::Begin, this));
}

void UCinematicContext::HandleOnFinished()
{
	RestoreStack.RestoreAll();
	UnbindDelegates();
	OnCinematicEnd.Broadcast(FCinematicContextEventPayload(ECinematicLifeCycle::End, this));
}

void UCinematicContext::UnbindDelegates()
{
	if (LevelSequencePlayer.IsValid())
	{
		LevelSequencePlayer->OnPlay.RemoveDynamic(this, &UCinematicContext::HandleOnPlay);
		LevelSequencePlayer->OnFinished.RemoveDynamic(this, &UCinematicContext::HandleOnFinished);
	}
}
