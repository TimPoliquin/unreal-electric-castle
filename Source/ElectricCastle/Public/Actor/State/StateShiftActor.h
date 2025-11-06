// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "StateShiftTypes.h"
#include "UObject/Interface.h"
#include "StateShiftActor.generated.h"

class USphereComponent;
class UStateShiftComponent;
// This class does not need to be modified.
UINTERFACE()
class UStateShiftActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API IStateShiftActor
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UShapeComponent* GetStateShiftCollisionComponent();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UStateShiftComponent* GetStateShiftComponent();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void StateShiftReaction_Visibility_Show();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void StateShiftReaction_Visibility_Hide();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void StateShiftReaction_Visibility_FadeIn();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void StateShiftReaction_Visibility_FadeOut();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void StateShiftReaction_Collision_Enable();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void StateShiftReaction_Collision_Disable();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void StateShiftReaction_Custom();

	static void EnableStateShiftCollision(UObject* Object);
};
