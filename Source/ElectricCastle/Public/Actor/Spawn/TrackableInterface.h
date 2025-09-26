// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TrackableInterface.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTrackableStopTrackingSignature, AActor*, Actor);

// This class does not need to be modified.
UINTERFACE()
class UTrackableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API ITrackableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual FOnTrackableStopTrackingSignature& GetStopTrackingDelegate() = 0;
};
