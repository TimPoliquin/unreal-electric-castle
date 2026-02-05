// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SummoningActor.generated.h"

class USummonComponent;
// This class does not need to be modified.
UINTERFACE()
class USummoningActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API ISummoningActor
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	USummonComponent* GetSummonComponent() const;

	static USummonComponent* GetSummonComponent(const UObject* Actor);
};
