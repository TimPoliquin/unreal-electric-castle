// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PoolableActor.generated.h"

class UPoolManagerComponent;

UINTERFACE()
class UPoolableActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API IPoolableActor
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UPoolManagerComponent* GetPoolManager() const;

	static UPoolManagerComponent* GetPoolManager(const AActor* Object);
	static void ReturnToPoolOrDestroy(AActor* Object);
};
