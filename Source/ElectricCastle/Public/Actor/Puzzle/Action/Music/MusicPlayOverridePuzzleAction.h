// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Actor/Puzzle/Action/PuzzleAction.h"
#include "MusicPlayOverridePuzzleAction.generated.h"

/**
 * 
 */
UCLASS(DisplayName="Puzzle Action|Music|Play Override Action")
class ELECTRICCASTLE_API UMusicPlayOverridePuzzleAction : public UPuzzleAction
{
	GENERATED_BODY()

public:
	virtual bool ShouldExecute_Implementation() const override;
	virtual void Execute_Implementation() const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties")
	TObjectPtr<USoundBase> MusicOverride;
};
