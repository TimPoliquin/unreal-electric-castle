// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "ElectricCastleAIController.generated.h"

class UCinematicHandlerComponent;
class UBehaviorTreeComponent;
class UBlackboardComponent;

UCLASS()
class ELECTRICCASTLE_API AElectricCastleAIController : public AAIController
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AElectricCastleAIController();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCinematicHandlerComponent> CinematicHandlerComponent;

	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;
};
