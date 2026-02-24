// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Cinematic/CinematicEvents.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "CinematicContext.generated.h"

USTRUCT()
struct FCinematicRestoreStack
{
	GENERATED_BODY()

	void Add(TFunction<void()> Func)
	{
		Actions.Add(MoveTemp(Func));
	}

	void RestoreAll()
	{
		for (int32 i = Actions.Num() - 1; i >= 0; --i) { Actions[i](); }
		Actions.Empty();
	}

private:
	TArray<TFunction<void()>> Actions;
};

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UCinematicContext : public UObject
{
	GENERATED_BODY()

public:
	ULevelSequencePlayer* GetLevelSequencePlayer() const;
	ULevelSequence* GetLevelSequence() const;
	FGameplayTagContainer GetCinematicTagContainer() const;
	void RestoreAll();

	void SetLevelSequencePlayer(ULevelSequencePlayer* InPlayer);
	void SetLevelSequence(ULevelSequence* InSequence);
	void SetCinematicTags(const FGameplayTagContainer& InCinematicTags);
	void AddRestoreFunction(const TFunction<void()>& InRestoreFunc);

	FCinematicContextEventSignature OnCinematicBegin;
	FCinematicContextEventSignature OnCinematicEnd;

protected:
	UPROPERTY()
	TWeakObjectPtr<ULevelSequencePlayer> LevelSequencePlayer;
	UPROPERTY()
	TWeakObjectPtr<ULevelSequence> LevelSequence;
	UPROPERTY()
	FGameplayTagContainer CinematicTags = FGameplayTagContainer::EmptyContainer;
	UPROPERTY()
	FCinematicRestoreStack RestoreStack;

private:
	/** Event callback functions */
	UFUNCTION()
	void HandleOnPlay();

	UFUNCTION()
	void HandleOnFinished();

	/** Unbind all delegates */
	void UnbindDelegates();
};
