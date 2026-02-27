// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Actor/Puzzle/Action/PuzzleAction.h"
#include "LockPuzzleAction.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API ULockPuzzleAction : public UPuzzleAction
{
	GENERATED_BODY()

public:
	virtual bool ShouldExecute_Implementation() const override;
	virtual void Execute_Implementation() const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties", meta=(MustImplement="/Script/ElectricCastle.LockInterface"))
	TArray<TObjectPtr<AActor>> ToLock;
};
