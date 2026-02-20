// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SelectionWheelManagerActorInterface.generated.h"

class USelectionWheelManagerComponent;
// This class does not need to be modified.
UINTERFACE()
class USelectionWheelManagerActorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API ISelectionWheelManagerActorInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	USelectionWheelManagerComponent* GetSelectionWheelManagerComponent() const;

	static USelectionWheelManagerComponent* GetSelectionWheelManagerComponent(const UObject* Object);
};
