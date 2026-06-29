// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TrackFollowingActor.generated.h"

class UTrackFollowingComponent;
// This class does not need to be modified.
UINTERFACE()
class UTrackFollowingActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API ITrackFollowingActor
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Track")
	UTrackFollowingComponent* GetTrackFollowingComponent() const;

	static UTrackFollowingComponent* GetTrackFollowingComponent(const UObject* Actor);
};
