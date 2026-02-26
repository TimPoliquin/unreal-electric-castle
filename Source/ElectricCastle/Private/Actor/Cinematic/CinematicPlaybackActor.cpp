// Copyright Alien Shores


#include "Actor/Cinematic/CinematicPlaybackActor.h"

#include "Cinematic/CinematicManager.h"
#include "Cinematic/Context/CinematicContext.h"


// Sets default values
ACinematicPlaybackActor::ACinematicPlaybackActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ACinematicPlaybackActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	CinematicContext = NewObject<UCinematicContext>(this);
	CinematicContext->SetSequenceMetadata(Metadata);
}

void ACinematicPlaybackActor::Start_Implementation()
{
	if (UCinematicManager* CinematicManager = UCinematicManager::Get(this))
	{
		CinematicManager->RegisterContext(CinematicContext);
	}
	bIsPlaying = true;
	bHasPlayed = true;
	CinematicContext->OnCinematicBegin.Broadcast(FCinematicContextEventPayload(ECinematicLifeCycle::Begin, CinematicContext));
}

void ACinematicPlaybackActor::End_Implementation()
{
	bIsPlaying = false;
	CinematicContext->RestoreAll();
	CinematicContext->OnCinematicEnd.Broadcast(FCinematicContextEventPayload(ECinematicLifeCycle::End, CinematicContext));
}

bool ACinematicPlaybackActor::IsTriggered_Implementation() const
{
	return bHasPlayed;
}

void ACinematicPlaybackActor::OnTrigger_Implementation(const bool InActivate)
{
	if (InActivate)
	{
		Start();
	}
	else
	{
		End();
	}
}
