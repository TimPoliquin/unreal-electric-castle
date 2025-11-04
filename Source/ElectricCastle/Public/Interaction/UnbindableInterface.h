// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UnbindableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UUnbindableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API IUnbindableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/**
	 * Unbind all delegates connected to the bound object
	 * @param BoundObject 
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta=(DefaultToSelf="BoundObject"))
	void UnbindAll(const UObject* BoundObject);
};
