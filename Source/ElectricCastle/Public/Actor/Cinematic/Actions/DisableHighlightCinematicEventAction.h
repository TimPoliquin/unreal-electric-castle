// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "CinematicEventAction.h"
#include "Components/ActorComponent.h"
#include "DisableHighlightCinematicEventAction.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API UDisableHighlightCinematicEventAction : public UCinematicEventAction
{
	GENERATED_BODY()

public:
	UDisableHighlightCinematicEventAction();
	virtual void Execute_Implementation(const UCinematicContextHandle* ContextHandle) const override;
};
