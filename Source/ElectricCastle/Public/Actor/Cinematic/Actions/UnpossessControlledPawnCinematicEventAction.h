// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "CinematicEventAction.h"
#include "UnpossessControlledPawnCinematicEventAction.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UUnpossessControlledPawnCinematicEventAction : public UCinematicEventAction
{
	GENERATED_BODY()

public:
	UUnpossessControlledPawnCinematicEventAction();
	virtual void Execute_Implementation(const UCinematicContextHandle* ContextHandle) const override;
};
