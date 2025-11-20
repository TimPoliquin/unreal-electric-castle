// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SocketManagerActor.generated.h"

class USocketManagerComponent;
// This class does not need to be modified.
UINTERFACE()
class USocketManagerActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API ISocketManagerActor
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	USocketManagerComponent* GetSocketManagerComponent() const;

	static USocketManagerComponent* GetSocketManagerComponent(const UObject* Actor);
};
