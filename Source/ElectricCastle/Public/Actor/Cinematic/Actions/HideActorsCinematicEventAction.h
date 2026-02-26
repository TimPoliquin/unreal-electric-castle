// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "CinematicEventAction.h"
#include "HideActorsCinematicEventAction.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class ELECTRICCASTLE_API UHideActorsCinematicEventAction : public UCinematicEventAction
{
	GENERATED_BODY()

public:
	virtual void Execute_Implementation(const UCinematicContextHandle* ContextHandle) const override;

protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void GetActorsToHide(TArray<AActor*>& OutActors) const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties")
	bool bHideChildActors = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties")
	bool bApplyRelocation = false;

private:
	void HideActor(AActor* Actor, const UCinematicContextHandle* ContextHandle) const;
	void RelocateActor(AActor* Actor, const UCinematicContextHandle* ContextHandle) const;
};
