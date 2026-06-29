// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CloakActorInterface.generated.h"

class UCloakComponent;
// This class does not need to be modified.
UINTERFACE()
class UCloakActorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API ICloakActorInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UCloakComponent* GetCloakComponent() const;

	static UCloakComponent* GetCloakComponent(const UObject* WorldContextObject);
};
