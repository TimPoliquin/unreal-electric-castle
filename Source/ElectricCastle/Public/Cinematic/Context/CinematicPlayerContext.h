// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "CinematicContext.h"
#include "CinematicPlayerContext.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UCinematicPlayerContext : public UCinematicContext
{
	GENERATED_BODY()

public:
	void SetLevelSequencePlayer(ULevelSequencePlayer* InPlayer);
	virtual UWorld* GetWorld() const override;

	static UCinematicPlayerContext* Create(ULevelSequencePlayer* InLevelSequencePlayer, const ULevelSequence* InLevelSequence);

private:
	UPROPERTY()
	TWeakObjectPtr<ULevelSequencePlayer> LevelSequencePlayer;

	/** Event callback functions */
	UFUNCTION()
	void HandleOnPlay();

	UFUNCTION()
	void HandleOnFinished();

	/** Unbind all delegates */
	void UnbindDelegates();
};
