// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MotionWarpingActor.generated.h"

class UMotionWarpingComponent;
// This class does not need to be modified.
UINTERFACE()
class UMotionWarpingActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API IMotionWarpingActor
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UMotionWarpingComponent* GetMotionWarpingComponent() const;

	static bool IsMotionWarping(const UObject* Object);
};
