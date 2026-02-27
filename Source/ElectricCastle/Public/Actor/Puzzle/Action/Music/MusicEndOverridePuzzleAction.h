// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Actor/Puzzle/Action/PuzzleAction.h"
#include "MusicEndOverridePuzzleAction.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UMusicEndOverridePuzzleAction : public UPuzzleAction
{
	GENERATED_BODY()

public:
	virtual void Execute_Implementation() const override;
};
