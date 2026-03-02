// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Actor/Puzzle/Action/PuzzleAction.h"
#include "CinematicSolverPuzzleAction.generated.h"

/**
 * 
 */
UCLASS(DisplayName="Puzzle Action|Solver|Cinematic Action")
class ELECTRICCASTLE_API UCinematicSolverPuzzleAction : public UPuzzleAction
{
	GENERATED_BODY()

public:
	virtual bool ShouldExecute_Implementation() const override;
	virtual void Execute_Implementation() const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties", meta=(MustImplement="/Script/ElectricCastle.CinematicPlaybackActorInterface"))
	TObjectPtr<AActor> CinematicPlaybackActor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
	bool bStartPlaybackOnExecute = true;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void HandlePlaybackStart();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void HandlePlaybackEnd();
};
