// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "CinematicEventAction.h"
#include "RemoveInputMappingContextCinematicEventAction.generated.h"

class UInputMappingContext;
/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API URemoveInputMappingContextCinematicEventAction : public UCinematicEventAction
{
	GENERATED_BODY()

public:
	URemoveInputMappingContextCinematicEventAction();
	virtual void Execute_Implementation(const UCinematicContextHandle* ContextHandle) const override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputMappingContext> InputMappingContext;
};
