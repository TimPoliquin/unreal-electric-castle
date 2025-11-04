// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_InsideProcessingRange.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UBTDecorator_InsideProcessingRange : public UBTDecorator
{
	GENERATED_BODY()
protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Selectors")
	FBlackboardKeySelector DistanceToTargetSelector;
};
