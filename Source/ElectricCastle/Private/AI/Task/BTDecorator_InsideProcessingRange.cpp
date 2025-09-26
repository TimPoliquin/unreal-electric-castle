// Copyright Alien Shores


#include "BTDecorator_InsideProcessingRange.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/EnemyInterface.h"


bool UBTDecorator_InsideProcessingRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if (const AAIController* AIController = OwnerComp.GetAIOwner())
	{
		if (const APawn* ControlledPawn = AIController->GetPawn())
		{
			if (const IEnemyInterface* EnemyInterface = Cast<IEnemyInterface>(ControlledPawn))
			{
				return OwnerComp.GetBlackboardComponent()->GetValueAsFloat(DistanceToTargetSelector.SelectedKeyName) < EnemyInterface->GetMaxAIProcessingRange();
			}
		}
	}
	return false;
}
