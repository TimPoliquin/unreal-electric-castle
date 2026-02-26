// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "CinematicEventAction.h"
#include "DisableTickCinematicEventAction.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class ELECTRICCASTLE_API UDisableTickCinematicEventAction : public UCinematicEventAction
{
	GENERATED_BODY()

public:
	UDisableTickCinematicEventAction();
	virtual void Execute_Implementation(const UCinematicContextHandle* ContextHandle) const override;

protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void GetActorsToDisableTick(TArray<AActor*>& OutActors) const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties")
	bool bDisableChildActors = false;

private:
	void DisableTick(AActor* Actor, const UCinematicContextHandle* ContextHandle) const;
};
