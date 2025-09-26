// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TieredItemInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UTieredItemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API ITieredItemInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetItemLevel() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetItemLevel(int32 Level);

	static void SetItemLevel(UObject* Object, int32 Level);
};
