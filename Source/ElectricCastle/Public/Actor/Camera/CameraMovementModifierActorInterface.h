// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CameraMovementModifierActorInterface.generated.h"

class UCameraMovementModifier;
// This class does not need to be modified.
UINTERFACE()
class UCameraMovementModifierActorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API ICameraMovementModifierActorInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UCameraMovementModifier* GetCameraMovementModifier() const;

	static UCameraMovementModifier* GetCameraMovementModifier(UObject* Actor);
};
