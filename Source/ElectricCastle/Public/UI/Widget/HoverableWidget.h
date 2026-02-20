// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HoverableWidget.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UHoverableWidget : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API IHoverableWidget
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Hover();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Unhover();

	static void Hover(UObject* Widget);
	static void Unhover(UObject* Widget);
};
