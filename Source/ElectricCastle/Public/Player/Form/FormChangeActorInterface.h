// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FormChangeActorInterface.generated.h"

class UPlayerFormChangeComponent;
// This class does not need to be modified.
UINTERFACE()
class UFormChangeActorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API IFormChangeActorInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UPlayerFormChangeComponent* GetFormChangeComponent() const;

	static UPlayerFormChangeComponent* GetFormChangeComponent(const UObject* Object);
};
