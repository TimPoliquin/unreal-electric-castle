// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MagicHandPossessable.generated.h"

class UMagicHandComponent;
class UTetherUpdateHandler;

UINTERFACE(MinimalAPI, Blueprintable)
class UMagicHandPossessable : public UInterface
{
	GENERATED_BODY()
};

class IMagicHandPossessable
{
	GENERATED_BODY()

public:
	// Whether this actor can be possessed right now. Called every time the projectile
	// overlaps the actor — return false to reject (e.g. already possessed, locked, etc.).
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Magic Hand")
	bool CanBePossessedByMagicHand() const;

	// Return the handler that will drive this actor's response to tether movement.
	// The handler is owned by this actor; MagicHandComponent caches a non-owning reference.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Magic Hand")
	UTetherUpdateHandler* GetTetherUpdateHandler();

	// Called once when possession begins. Use this to apply visual effects, disable
	// physics simulation, etc.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Magic Hand")
	void OnPossessionStarted(const FMagicHandPossessEventPayload& Payload);

	// Called once when the tether is released (or broken). Mirror of OnPossessionStarted.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Magic Hand")
	void OnPossessionEnded(const FMagicHandPossessEventPayload& Payload);
};
