// Copyright Alien Shores


#include "Cinematic/Context/CinematicPlayerContext.h"
#include "Cinematic/Metadata/CinematicSequenceMetaData.h"

#include "LevelSequencePlayer.h"

void UCinematicPlayerContext::SetLevelSequencePlayer(ULevelSequencePlayer* InPlayer)
{
	LevelSequencePlayer = InPlayer;
	if (!IsValid(InPlayer))
	{
		return;
	}
	InPlayer->OnPlay.AddDynamic(this, &UCinematicPlayerContext::HandleOnPlay);
	InPlayer->OnFinished.AddDynamic(this, &UCinematicPlayerContext::HandleOnFinished);
	InPlayer->OnStop.AddDynamic(this, &UCinematicPlayerContext::HandleOnFinished);
}

UWorld* UCinematicPlayerContext::GetWorld() const
{
	return LevelSequencePlayer.IsValid() ? LevelSequencePlayer->GetWorld() : Super::GetWorld();
}

UCinematicPlayerContext* UCinematicPlayerContext::Create(ULevelSequencePlayer* InLevelSequencePlayer, const ULevelSequence* InLevelSequence)
{
	UCinematicPlayerContext* Context = NewObject<UCinematicPlayerContext>(InLevelSequencePlayer, StaticClass());
	Context->SetLevelSequencePlayer(InLevelSequencePlayer);
	Context->SetSequenceMetadata(InLevelSequence->FindMetaData<UCinematicSequenceMetadata>());
	return Context;
}

void UCinematicPlayerContext::HandleOnPlay()
{
	OnCinematicBegin.Broadcast(FCinematicContextEventPayload(ECinematicLifeCycle::Begin, this));
}

void UCinematicPlayerContext::HandleOnFinished()
{
	RestoreStack.RestoreAll();
	UnbindDelegates();
	OnCinematicEnd.Broadcast(FCinematicContextEventPayload(ECinematicLifeCycle::End, this));
}

void UCinematicPlayerContext::UnbindDelegates()
{
	if (LevelSequencePlayer.IsValid())
	{
		LevelSequencePlayer->OnPlay.RemoveDynamic(this, &UCinematicPlayerContext::HandleOnPlay);
		LevelSequencePlayer->OnFinished.RemoveDynamic(this, &UCinematicPlayerContext::HandleOnFinished);
	}
}
