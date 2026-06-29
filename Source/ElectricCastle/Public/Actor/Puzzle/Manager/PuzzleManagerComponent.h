// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PuzzleManagerTypes.h"
#include "PuzzleManagerComponent.generated.h"

class UPuzzleAction;

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ELECTRICCASTLE_API UPuzzleManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	UPuzzleManagerComponent();
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ActivatePuzzle();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SolvePuzzle();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Puzzle")
	TArray<TObjectPtr<UPuzzleAction>> OnActivatePuzzleActions;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Puzzle")
	TArray<TObjectPtr<UPuzzleAction>> OnSolvePuzzleActions;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Puzzle", Replicated, SaveGame)
	EPuzzleManagerState PuzzleState = EPuzzleManagerState::Inactive;
};
