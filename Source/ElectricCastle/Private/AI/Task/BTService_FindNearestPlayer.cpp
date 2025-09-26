// Copyright Alien Shores


#include "AI/Task/BTService_FindNearestPlayer.h"

#include "AIController.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Game/Subsystem/ElectricCastleAIDirectorGameInstanceSubsystem.h"

UBTService_FindNearestPlayer::UBTService_FindNearestPlayer()
{
	NodeName = "Find Nearest Player";
}

void UBTService_FindNearestPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	UElectricCastleAIDirectorGameInstanceSubsystem* AIDirectorGameInstanceSubsystem = UElectricCastleAIDirectorGameInstanceSubsystem::Get(OwnerComp.GetAIOwner());
	if (!AIDirectorGameInstanceSubsystem)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s] No AIDirector subsystem - aborting"), *GetName())
		return;
	}
	float ClosestDistance = FLT_MAX;
	AActor* NearestPlayer = nullptr;
	if (const AAIController* AIController = OwnerComp.GetAIOwner())
	{
		if (const APawn* ControlledPawn = AIController->GetPawn())
		{
			const TArray<AActor*>& PlayerActors = AIDirectorGameInstanceSubsystem->GetActivePlayerActors();
			for (AActor* PlayerActor : PlayerActors)
			{
				const float Distance = FVector::Dist(ControlledPawn->GetActorLocation(), PlayerActor->GetActorLocation());
				if (Distance < ClosestDistance)
				{
					ClosestDistance = Distance;
					NearestPlayer = PlayerActor;
				}
			}
		}
	}
	if (OwnerComp.GetBlackboardComponent())
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(TargetToFollowSelector.SelectedKeyName, NearestPlayer);
		OwnerComp.GetBlackboardComponent()->SetValueAsFloat(DistanceToTargetSelector.SelectedKeyName, ClosestDistance);
	}
}
