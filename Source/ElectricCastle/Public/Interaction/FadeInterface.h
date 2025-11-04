// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FadeInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UFadeInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API IFadeInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void FadeIn();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void FadeOut();
};
