// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TriggerAction.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class ELECTRICCASTLE_API UTriggerAction : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void Execute();

protected:
	UFUNCTION(BlueprintNativeEvent)
	bool ShouldExecuteTrigger(const AActor* InTriggerActor) const;
	UFUNCTION(BlueprintNativeEvent)
	void ExecuteAction(AActor* InTriggerActor);

	UPROPERTY()
	TObjectPtr<AActor> OwnerActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Properties")
	TObjectPtr<AActor> TriggerActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Properties")
	bool bDebug = false;
};
