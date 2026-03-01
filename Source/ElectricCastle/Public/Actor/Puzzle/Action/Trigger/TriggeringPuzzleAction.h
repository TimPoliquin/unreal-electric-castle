// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Actor/Puzzle/Action/PuzzleAction.h"
#include "Actor/Puzzle/Manager/PuzzleManagerTypes.h"
#include "TriggeringPuzzleAction.generated.h"

/**
 * 
 */
UCLASS(DisplayName="Puzzle Action|Trigger|Trigger Action")
class ELECTRICCASTLE_API UTriggeringPuzzleAction : public UPuzzleAction
{
	GENERATED_BODY()

public:
	virtual void Execute_Implementation() const override;

	static UTriggeringPuzzleAction* Create(UObject* Outer, FName Name, EPuzzleActionTriggerMode InTriggerMode);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties", meta=(MustImplement="/Script/ElectricCastle.TriggerInterface"))
	TArray<AActor*> TriggerActors;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
	EPuzzleActionTriggerMode TriggerMode = EPuzzleActionTriggerMode::Activate;
};
