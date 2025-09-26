// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FishingSuccessWidgetInterface.generated.h"

class UFishCatch;
// This class does not need to be modified.
UINTERFACE()
class UFishingSuccessWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API IFishingSuccessWidgetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetFishingCatchInfo(const UFishCatch* FishCatch);

	static void SetFishingCatchInfo(UObject* FishingSuccessWidget, const UFishCatch* FishCatch);
};
