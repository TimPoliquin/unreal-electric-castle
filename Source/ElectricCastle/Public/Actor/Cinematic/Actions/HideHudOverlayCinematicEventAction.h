// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "CinematicEventAction.h"
#include "HideHudOverlayCinematicEventAction.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UHideHudOverlayCinematicEventAction : public UCinematicEventAction
{
	GENERATED_BODY()

public:
	UHideHudOverlayCinematicEventAction();
	virtual void Execute_Implementation(const UCinematicContextHandle* ContextHandle) const override;
};
