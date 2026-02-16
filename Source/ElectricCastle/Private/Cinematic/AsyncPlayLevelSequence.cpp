// Copyright Alien Shores


#include "Cinematic/AsyncPlayLevelSequence.h"

#include "Engine/World.h"
#include "LevelSequence.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"

UAsyncPlayLevelSequence* UAsyncPlayLevelSequence::AsyncPlayLevelSequence(
	UObject* WorldContextObject,
	ULevelSequence* LevelSequence,
	FMovieSceneSequencePlaybackSettings Settings,
	ALevelSequenceActor*& OutActor)
{
	UAsyncPlayLevelSequence* Node = NewObject<UAsyncPlayLevelSequence>();

	if (WorldContextObject)
	{
		Node->WorldPtr = WorldContextObject->GetWorld();
	}

	Node->Sequence = LevelSequence;
	Node->PlaybackSettings = Settings;
	OutActor = nullptr; // Will be set in Activate

	return Node;
}

void UAsyncPlayLevelSequence::Activate()
{
	Super::Activate();

	UWorld* World = WorldPtr.Get();
	if (!World || !Sequence)
	{
		OnFinished.Broadcast();
		SetReadyToDestroy();
		return;
	}

	// Spawn the Level Sequence Actor
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	SequenceActor = World->SpawnActor<ALevelSequenceActor>(ALevelSequenceActor::StaticClass(), SpawnParams);

	if (!SequenceActor)
	{
		OnFinished.Broadcast();
		SetReadyToDestroy();
		return;
	}

	// Set the sequence
	SequenceActor->SetSequence(Sequence);
	SequenceActor->PlaybackSettings = PlaybackSettings;

	// Initialize the sequence player
	SequenceActor->InitializePlayer();
	SequencePlayer = SequenceActor->GetSequencePlayer();

	if (!SequencePlayer)
	{
		CleanupSequenceActor();
		OnFinished.Broadcast();
		SetReadyToDestroy();
		return;
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
	OnPlay.Broadcast();
}

void UAsyncPlayLevelSequence::HandleOnPause()
{
	OnPause.Broadcast();
}

void UAsyncPlayLevelSequence::HandleOnStop()
{
	OnStop.Broadcast();
}

void UAsyncPlayLevelSequence::HandleOnCameraCut(UCameraComponent* CameraComponent)
{
	OnCameraCut.Broadcast();
}

void UAsyncPlayLevelSequence::HandleOnPlayReverse()
{
	OnPlayReverse.Broadcast();
}

void UAsyncPlayLevelSequence::HandleOnFinished()
{
	OnFinished.Broadcast();

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
