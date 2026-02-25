// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CinematicHandlerInterface.generated.h"

class UCinematicContextHandle;
class UCinematicHandlerComponent;
// This class does not need to be modified.
UINTERFACE()
class UCinematicHandlerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API ICinematicHandlerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UCinematicHandlerComponent* GetCinematicHandlerComponent();

	static bool IsCinematicHandler(const UObject* Object);
	static UCinematicHandlerComponent* GetCinematicHandlerComponent(UObject* Object);
};
