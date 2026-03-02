// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Actor/Puzzle/Action/PuzzleAction.h"
#include "UnlockPuzzleAction.generated.h"

/**
 * 
 */
UCLASS(DisplayName="Puzzle Action|Lock|Unlock Action")
class ELECTRICCASTLE_API UUnlockPuzzleAction : public UPuzzleAction
{
	GENERATED_BODY()

public:
	virtual bool ShouldExecute_Implementation() const override;
	virtual void Execute_Implementation() const override;

protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UnlockActor(AActor* Actor) const;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties", meta=(MustImplement="/Script/ElectricCastle.LockInterface"))
	TArray<TObjectPtr<AActor>> ToUnlock;
};
