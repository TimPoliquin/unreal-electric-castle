// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "CinematicHandlerTypes.h"
#include "UObject/Interface.h"
#include "CinematicPlaybackActorInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UCinematicPlaybackActorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API ICinematicPlaybackActorInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Start();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void End();

	virtual FCinematicPlaybackEventSignature& GetOnPlaybackStartDelegate() = 0;
	virtual FCinematicPlaybackEventSignature& GetOnPlaybackEndDelegate() = 0;

	static void Start(UObject* Actor);
	static void End(UObject* Actor);
};
