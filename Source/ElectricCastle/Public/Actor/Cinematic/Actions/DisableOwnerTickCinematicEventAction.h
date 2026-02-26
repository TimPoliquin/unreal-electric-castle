// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "DisableTickCinematicEventAction.h"
#include "DisableOwnerTickCinematicEventAction.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UDisableOwnerTickCinematicEventAction : public UDisableTickCinematicEventAction
{
	GENERATED_BODY()

protected:
	virtual void GetActorsToDisableTick_Implementation(TArray<AActor*>& OutActors) const override;
};
