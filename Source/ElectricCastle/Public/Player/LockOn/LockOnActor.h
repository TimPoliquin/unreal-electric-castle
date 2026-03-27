// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LockOnActor.generated.h"

class ULockOnController;
// This class does not need to be modified.
UINTERFACE()
class ULockOnActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API ILockOnActor
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	ULockOnController* GetLockOnController() const;

	static ULockOnController* GetLockOnController(const UObject* Actor);
};
