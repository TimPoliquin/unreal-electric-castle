// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TetherAbilityActorInterface.generated.h"

class UTetherAbilityComponent;
// This class does not need to be modified.
UINTERFACE()
class UTetherAbilityActorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API ITetherAbilityActorInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UTetherAbilityComponent* GetTetherAbilityComponent() const;

	static UTetherAbilityComponent* GetTetherAbilityComponent(const UObject* Actor);
};
