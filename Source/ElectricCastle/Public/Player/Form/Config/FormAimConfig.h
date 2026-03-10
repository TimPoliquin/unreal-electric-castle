// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "FormConfig.h"
#include "GameplayTagContainer.h"
#include "ScalableFloat.h"
#include "FormAimConfig.generated.h"

struct FGameplayTag;
/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UFormAimConfig : public UFormConfig
{
	GENERATED_BODY()

public:
	UFormAimConfig();
	virtual void OnFormActivated_Implementation(AActor* Actor, UPlayerFormChangeComponent* FormChangeComponent) override;
	virtual void OnFormDeactivated_Implementation(AActor* Actor, UPlayerFormChangeComponent* FormChangeComponent) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag AimAbilityTag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bShouldShowCrosshair = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FScalableFloat TraceDistance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FScalableFloat TraceRadius;
};
