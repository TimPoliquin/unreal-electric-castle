// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PuzzleManagerActorInterface.generated.h"

class UPuzzleManagerComponent;
// This class does not need to be modified.
UINTERFACE()
class UPuzzleManagerActorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API IPuzzleManagerActorInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UPuzzleManagerComponent* GetPuzzleManagerComponent() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void HandlePuzzleActivated();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void HandlePuzzleSolved();

	static bool IsPuzzleManagerActor(const UObject* Object);
	static UPuzzleManagerComponent* GetPuzzleManagerComponent(const UObject* Object);
	static void ActivatePuzzle(const UObject* PuzzleActor);
	static void SolvePuzzle(const UObject* PuzzleActor);
	static void HandlePuzzleActivated(UObject* PuzzleActor);
	static void HandlePuzzleSolved(UObject* PuzzleActor);
};
