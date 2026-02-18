// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RadialInputDispatcherInterface.generated.h"

class URadialUIInputComponent;
// This class does not need to be modified.
UINTERFACE()
class URadialInputDispatcherInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API IRadialInputDispatcherInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	URadialUIInputComponent* GetRadialUIInputComponent() const;

	static URadialUIInputComponent* GetRadialUIInputComponent(const UObject* Object);
};
