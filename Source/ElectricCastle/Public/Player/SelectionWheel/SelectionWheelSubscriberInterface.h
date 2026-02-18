// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SelectionWheelSubscriberInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class USelectionWheelSubscriberInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API ISelectionWheelSubscriberInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnSelectionWheelAngleChange(float Value);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnSelectionWheelConfirm();

	static bool IsSelectionWheelSubscriber(const UObject* Object);
	static void OnSelectionWheelAngleChange(UObject* Object, const float Value);
	static void OnSelectionWheelConfirm(UObject* Object);
};
