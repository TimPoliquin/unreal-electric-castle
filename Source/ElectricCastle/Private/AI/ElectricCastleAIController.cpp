// Copyright Alien Shores


#include "AI/ElectricCastleAIController.h"

#include "Actor/Cinematic/CinematicHandlerComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"


// Sets default values
AElectricCastleAIController::AElectricCastleAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackoardComponent"));
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	CinematicHandlerComponent = CreateDefaultSubobject<UCinematicHandlerComponent>(TEXT("CinematicHandlerComponent"));
}
