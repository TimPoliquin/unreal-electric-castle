// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Actor/Horde/HordeModeComponent.h"
#include "Actor/Puzzle/Action/PuzzleAction.h"
#include "HordeModeSolverPuzzleAction.generated.h"

/**
 * 
 */
UCLASS(DisplayName="Puzzle Action|Solver|Horde Mode Action")
class ELECTRICCASTLE_API UHordeModeSolverPuzzleAction : public UPuzzleAction
{
	GENERATED_BODY()

public:
	virtual bool ShouldExecute_Implementation() const override;
	virtual void Execute_Implementation() const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties", meta=(MustImplement="/Script/ElectricCastle.HordeModeActorInterface"))
	TObjectPtr<AActor> HordeModeActor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties")
	bool bStartHordeOnExecute = true;

private:
	UFUNCTION()
	void OnHordeModeRoundComplete(const FOnHordeModeRoundCompletePayload& Payload);
};
