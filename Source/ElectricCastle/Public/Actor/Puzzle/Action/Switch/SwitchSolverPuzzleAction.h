// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Actor/Puzzle/Action/PuzzleAction.h"
#include "Actor/Switch/SwitchDelegates.h"
#include "SwitchSolverPuzzleAction.generated.h"

class AButtonSwitchActor;
/**
 * This puzzle action solves the puzzle when the button is activated
 */
UCLASS()
class ELECTRICCASTLE_API USwitchSolverPuzzleAction : public UPuzzleAction
{
	GENERATED_BODY()

public:
	virtual bool ShouldExecute_Implementation() const override;
	virtual void Execute_Implementation() const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties", meta=(MustImplement="/Script/ElectricCastle.SwitchInterface"))
	TObjectPtr<AActor> Switch;
	UFUNCTION(BlueprintNativeEvent)
	void HandleButtonActivated(const FOnSwitchStatusChangedPayload& Payload);
};
