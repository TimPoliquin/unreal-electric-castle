// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BlockingActorInterface.generated.h"

class UBlockController;
// This class does not need to be modified.
UINTERFACE()
class UBlockingActorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API IBlockingActorInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UBlockController* GetBlockController() const;

	static UBlockController* GetBlockController(const UObject* Object);
};
