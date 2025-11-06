// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "StateShiftTypes.h"
#include "UObject/Interface.h"
#include "StateShiftInterface.generated.h"

class USphereComponent;
class UStateShiftComponent;
// This class does not need to be modified.
UINTERFACE()
class UStateShiftInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API IStateShiftInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual UStateShiftComponent* GetStateShiftComponent() const = 0;
	virtual UShapeComponent* GetStateShiftCollisionComponent() const = 0;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void StateShiftReaction(const FStateShiftStateChangedPayload& Payload);

	static UStateShiftComponent* GetStateShiftComponent(UObject* Object);
};
