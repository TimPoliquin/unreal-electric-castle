// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "CinematicEventAction.h"
#include "DisableTriggerCinematicEventAction.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UDisableTriggerCinematicEventAction : public UCinematicEventAction
{
	GENERATED_BODY()

public:
	UDisableTriggerCinematicEventAction();
	virtual void Execute_Implementation(const UCinematicContextHandle* ContextHandle) const override;
};
