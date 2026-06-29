// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TriggerableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UTriggerableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API ITriggerableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Trigger")
	void OnTrigger(const bool InActivate);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Trigger")
	bool IsTriggered() const;

	static void Trigger(UObject* TriggerActor, const bool InActivate);
	static bool IsTrigger(const UObject* Object);
	static bool IsTriggered(const UObject* Object);
};
