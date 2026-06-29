// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AIPerceptionManagerActor.generated.h"

class UAIPerceptionManager;
// This class does not need to be modified.
UINTERFACE()
class UAIPerceptionManagerActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API IAIPerceptionManagerActor
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UAIPerceptionManager* GetAIPerceptionManager() const;

	static UAIPerceptionManager* GetAIPerceptionManager(const UObject* WorldContext);
};
