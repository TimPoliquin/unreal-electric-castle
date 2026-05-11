// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AIPerceptionActor.generated.h"

class UAIPerceptionComponent;
// This class does not need to be modified.
UINTERFACE()
class UAIPerceptionActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API IAIPerceptionActor
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UAIPerceptionComponent* GetAIPerceptionComponent() const;

	static bool IsAIPerceptionActor(const UObject* Actor);
	static UAIPerceptionComponent* GetAIPerceptionComponent(const UObject* Actor);
};
