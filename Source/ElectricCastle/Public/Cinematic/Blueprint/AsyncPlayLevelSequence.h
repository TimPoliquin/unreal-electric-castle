// Copyright Alien Shores

// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncPlayLevelSequence.generated.h"

class ULevelSequencePlayer;
class UCameraComponent;
class ALevelSequenceActor;
struct FMovieSceneSequencePlaybackSettings;
class ULevelSequence;

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FLevelSequenceEventPayload
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ALevelSequenceActor> SequenceActor;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ULevelSequencePlayer> SequencePlayer;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ULevelSequence> Sequence;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelSequenceEvent, const FLevelSequenceEventPayload&, Payload);

/**
 * Async Blueprint node that spawns a Level Sequence Player and provides execution pins
 * for all major playback events (Play, Pause, Stop, Camera Cut, Play Reverse, Finished)
 */
UCLASS()
class ELECTRICCASTLE_API UAsyncPlayLevelSequence : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	/** Called when the sequence starts playing */
	UPROPERTY(BlueprintAssignable)
	FOnLevelSequenceEvent OnPlay;

	/** Called when the sequence is paused */
	UPROPERTY(BlueprintAssignable)
	FOnLevelSequenceEvent OnPause;

	/** Called when a camera cut occurs */
	UPROPERTY(BlueprintAssignable)
	FOnLevelSequenceEvent OnCameraCut;

	/** Called when the sequence stops */
	UPROPERTY(BlueprintAssignable)
	FOnLevelSequenceEvent OnStop;

	/** Called when the sequence plays in reverse */
	UPROPERTY(BlueprintAssignable)
	FOnLevelSequenceEvent OnPlayReverse;

	/** Called when the sequence finishes playing */
	UPROPERTY(BlueprintAssignable)
	FOnLevelSequenceEvent OnFinished;

	/**
	 * Spawns a Level Sequence Player and plays the specified sequence.
	 * Provides execution pins for all playback events.
	 * 
	 * @param WorldContextObject - World context for spawning the actor
	 * @param LevelSequence - The level sequence asset to play
	 * @param Settings - Playback settings for the sequence
	 * @return The async action node
	 */
	UFUNCTION(BlueprintCallable, Category = "Cinematics", meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", DisplayName="Play Level Sequence with Finish"))
	static UAsyncPlayLevelSequence* AsyncPlayLevelSequence(
		UObject* WorldContextObject,
		ULevelSequence* LevelSequence,
		FMovieSceneSequencePlaybackSettings Settings
	);

	// UBlueprintAsyncActionBase interface
	virtual void Activate() override;

	/** Cleanup function called when task is destroyed */
	virtual void BeginDestroy() override;

private:
	/** Reference to the world */
	UPROPERTY()
	TWeakObjectPtr<UWorld> WorldPtr;

	/** Reference to the spawned Level Sequence Actor */
	UPROPERTY()
	TObjectPtr<ALevelSequenceActor> SequenceActor;

	/** Reference to the Level Sequence Player */
	UPROPERTY()
	TObjectPtr<ULevelSequencePlayer> SequencePlayer;

	/** The level sequence to play */
	UPROPERTY()
	TObjectPtr<ULevelSequence> Sequence;

	/** Playback settings */
	FMovieSceneSequencePlaybackSettings PlaybackSettings;

	/** Event callback functions */
	UFUNCTION()
	void HandleOnPlay();

	UFUNCTION()
	void HandleOnPause();

	UFUNCTION()
	void HandleOnStop();

	UFUNCTION()
	void HandleOnCameraCut(UCameraComponent* CameraComponent);

	UFUNCTION()
	void HandleOnFinished();

	UFUNCTION()
	void HandleOnPlayReverse();

	/** Unbind all delegates */
	void UnbindDelegates();

	/** Destroy the sequence actor if it exists */
	void CleanupSequenceActor();
};
