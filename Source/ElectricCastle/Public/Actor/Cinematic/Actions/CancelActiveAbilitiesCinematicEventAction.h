// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "CinematicEventAction.h"
#include "CancelActiveAbilitiesCinematicEventAction.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UCancelActiveAbilitiesCinematicEventAction : public UCinematicEventAction
{
	GENERATED_BODY()

public:
	UCancelActiveAbilitiesCinematicEventAction();

	virtual void Execute_Implementation(const UCinematicContextHandle* ContextHandle) const override;
};
