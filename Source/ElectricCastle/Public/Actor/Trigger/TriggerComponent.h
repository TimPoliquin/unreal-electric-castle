// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TriggerComponent.generated.h"

class UTriggerAction;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API UTriggerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTriggerComponent();
	
	UFUNCTION(BlueprintCallable)
	void ExecuteTriggerActions() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Export, Category="Trigger")
	TArray<TObjectPtr<UTriggerAction>> Actions;
};
