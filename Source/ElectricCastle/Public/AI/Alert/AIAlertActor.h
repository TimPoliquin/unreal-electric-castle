// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AIAlertActor.generated.h"

class UAIAlertComponent;
// This class does not need to be modified.
UINTERFACE()
class UAIAlertActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API IAIAlertActor
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UAIAlertComponent* GetAIAlertComponent() const;

	static UAIAlertComponent* GetAIAlertComponent(const UObject* Actor);
};
