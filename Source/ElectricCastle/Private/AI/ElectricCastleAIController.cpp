// Copyright Alien Shores


#include "AI/ElectricCastleAIController.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"


// Sets default values
AElectricCastleAIController::AElectricCastleAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackoardComponent"));
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
}

// Called when the game starts or when spawned
void AElectricCastleAIController::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AElectricCastleAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
