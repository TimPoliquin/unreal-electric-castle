// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AITargetingActorInterface.generated.h"

class UAITargetingComponent;
// This class does not need to be modified.
UINTERFACE()
class UAITargetingActorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API IAITargetingActorInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UAITargetingComponent* GetAITargetingComponent() const;

	static UAITargetingComponent* GetAITargetingComponent(const UObject* Object);
};
