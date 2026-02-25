// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "CinematicEventAction.h"
#include "HideActorsCinematicEventAction.h"
#include "HidePlayerCinematicEventAction.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UHidePlayerCinematicEventAction : public UHideActorsCinematicEventAction
{
	GENERATED_BODY()

public:
	UHidePlayerCinematicEventAction();

protected:
	virtual void GetActorsToHide_Implementation(TArray<AActor*>& OutActors) const override;
};
