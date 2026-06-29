// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LockInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class ULockInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API ILockInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Lock")
	void Lock();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Lock")
	void Unlock();

	static void Lock(UObject* Actor);
	static void Unlock(UObject* Gate);
};
