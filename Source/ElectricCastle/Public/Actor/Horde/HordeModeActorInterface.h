// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HordeModeActorInterface.generated.h"

class UHordeModeComponent;
// This class does not need to be modified.
UINTERFACE()
class UHordeModeActorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API IHordeModeActorInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UHordeModeComponent* GetHordeModeComponent() const;

	static UHordeModeComponent* GetHordeModeComponent(UObject* Object);
};
