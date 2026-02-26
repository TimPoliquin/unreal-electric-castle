// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "CinematicEventAction.h"
#include "HideActorsCinematicEventAction.h"
#include "HideOwnerActorCinematicEventAction.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UHideOwnerActorCinematicEventAction : public UHideActorsCinematicEventAction
{
	GENERATED_BODY()

public:
	UHideOwnerActorCinematicEventAction();

protected:
	virtual void GetActorsToHide_Implementation(TArray<AActor*>& OutActors) const override;
};
