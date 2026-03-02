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
UCLASS(DisplayName="Puzzle Action|Solver|Switch Solver")
class ELECTRICCASTLE_API USwitchSolverPuzzleAction : public UPuzzleAction
{
	GENERATED_BODY()

public:
	virtual bool ShouldExecute_Implementation() const override;
	virtual void Execute_Implementation() const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties", meta=(ObjectMustImplement="/Script/ElectricCastle.SwitchInterface"))
	TArray<TObjectPtr<AActor>> Switches;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties")
	bool bOrdered = false;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void HandleButtonActivated(const FOnSwitchStatusChangedPayload& Payload);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void MarkPuzzleSolved();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ResetSwitches();
};
