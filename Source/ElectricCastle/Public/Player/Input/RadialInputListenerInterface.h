// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RadialInputListenerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class URadialInputListenerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API IRadialInputListenerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnRadialInputAngleChange(float Value);

	static bool IsRadialInputListener(const UObject* Object);
	static void OnRadialInputAngleChange(UObject* Object, const float Value);
};
