// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_FindNearestPlayer.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UBTService_FindNearestPlayer : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_FindNearestPlayer();
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Selectors")
	FBlackboardKeySelector TargetToFollowSelector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Selectors")
	FBlackboardKeySelector DistanceToTargetSelector;
};
