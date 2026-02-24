// Copyright Alien Shores


#include "Cinematic/Blueprint/AsyncPlayLevelSequence.h"

#include "Engine/World.h"
#include "LevelSequence.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "Cinematic/CinematicManager.h"

UAsyncPlayLevelSequence* UAsyncPlayLevelSequence::AsyncPlayLevelSequence(
	UObject* WorldContextObject,
	ULevelSequence* LevelSequence,
	FMovieSceneSequencePlaybackSettings Settings
)
{
	UAsyncPlayLevelSequence* Node = NewObject<UAsyncPlayLevelSequence>();

	if (WorldContextObject)
	{
		Node->WorldPtr = WorldContextObject->GetWorld();
	}

	Node->Sequence = LevelSequence;
	Node->PlaybackSettings = Settings;

	return Node;
}

void UAsyncPlayLevelSequence::Activate()
{
	Super::Activate();

	UWorld* World = WorldPtr.Get();
	if (!World || !Sequence)
	{
		OnFinished.Broadcast(FLevelSequenceEventPayload(SequenceActor, SequencePlayer, Sequence));
		SetReadyToDestroy();
		return;
	}

	// Spawn the Level Sequence Actor
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	SequenceActor = World->SpawnActor<ALevelSequenceActor>(ALevelSequenceActor::StaticClass(), SpawnParams);

	if (!SequenceActor)
	{
		OnFinished.Broadcast(FLevelSequenceEventPayload(SequenceActor, SequencePlayer, Sequence));
		SetReadyToDestroy();
		return;
	}

	// Set the sequence
	SequenceActor->SetSequence(Sequence);

	// Initialize the sequence player
	SequenceActor->InitializePlayer();
	SequencePlayer = SequenceActor->GetSequencePlayer();
	SequencePlayer->SetPlaybackSettings(PlaybackSettings);

	if (!SequencePlayer)
	{
		CleanupSequenceActor();
		OnFinished.Broadcast(FLevelSequenceEventPayload(SequenceActor, SequencePlayer, Sequence));
		SetReadyToDestroy();
		return;
	}

	// register to cinematic manager
	if (UCinematicManager* CinematicManager = UCinematicManager::Get(World))
	{
		CinematicManager->RegisterSequencePlayer(SequencePlayer, Sequence);
	}

	// Bind to all events
	SequencePlayer->OnPlay.AddDynamic(this, &UAsyncPlayLevelSequence::HandleOnPlay);
	SequencePlayer->OnPause.AddDynamic(this, &UAsyncPlayLevelSequence::HandleOnPause);
	SequencePlayer->OnStop.AddDynamic(this, &UAsyncPlayLevelSequence::HandleOnStop);
	SequencePlayer->OnFinished.AddDynamic(this, &UAsyncPlayLevelSequence::HandleOnFinished);

	// Bind to camera cut events
	SequencePlayer->OnCameraCut.AddDynamic(this, &UAsyncPlayLevelSequence::HandleOnCameraCut);


	// Start playing the sequence
	SequencePlayer->Play();
}

void UAsyncPlayLevelSequence::HandleOnPlay()
{
	OnPlay.Broadcast(FLevelSequenceEventPayload(SequenceActor, SequencePlayer, Sequence));
}

void UAsyncPlayLevelSequence::HandleOnPause()
{
	OnPause.Broadcast(FLevelSequenceEventPayload(SequenceActor, SequencePlayer, Sequence));
}

void UAsyncPlayLevelSequence::HandleOnStop()
{
	OnStop.Broadcast(FLevelSequenceEventPayload(SequenceActor, SequencePlayer, Sequence));
}

void UAsyncPlayLevelSequence::HandleOnCameraCut(UCameraComponent* CameraComponent)
{
	OnCameraCut.Broadcast(FLevelSequenceEventPayload(SequenceActor, SequencePlayer, Sequence));
}

void UAsyncPlayLevelSequence::HandleOnPlayReverse()
{
	OnPlayReverse.Broadcast(FLevelSequenceEventPayload(SequenceActor, SequencePlayer, Sequence));
}

void UAsyncPlayLevelSequence::HandleOnFinished()
{
	OnFinished.Broadcast(FLevelSequenceEventPayload(SequenceActor, SequencePlayer, Sequence));

	// Clean up after sequence finishes
	UnbindDelegates();
	CleanupSequenceActor();
	SetReadyToDestroy();
}

void UAsyncPlayLevelSequence::UnbindDelegates()
{
	if (SequencePlayer)
	{
		SequencePlayer->OnPlay.RemoveDynamic(this, &UAsyncPlayLevelSequence::HandleOnPlay);
		SequencePlayer->OnPause.RemoveDynamic(this, &UAsyncPlayLevelSequence::HandleOnPause);
		SequencePlayer->OnStop.RemoveDynamic(this, &UAsyncPlayLevelSequence::HandleOnStop);
		SequencePlayer->OnFinished.RemoveDynamic(this, &UAsyncPlayLevelSequence::HandleOnFinished);
		SequencePlayer->OnCameraCut.RemoveDynamic(this, &UAsyncPlayLevelSequence::HandleOnCameraCut);
	}
}

void UAsyncPlayLevelSequence::CleanupSequenceActor()
{
	if (SequenceActor && SequenceActor->IsValidLowLevel())
	{
		SequenceActor->Destroy();
		SequenceActor = nullptr;
	}
}

void UAsyncPlayLevelSequence::BeginDestroy()
{
	// Ensure cleanup when the task is destroyed
	UnbindDelegates();
	CleanupSequenceActor();

	Super::BeginDestroy();
}
