// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Alert/AlertTypes.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Enum.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Float.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Engagement/EngagementTypes.h"
#include "Player/LockOn/LockOnEvents.h"
#include "Utils/RandomRange.h"
#include "ElectricCastleAIController.generated.h"

class UCinematicHandlerComponent;
class UBehaviorTreeComponent;
class UBlackboardComponent;
class UBlackboardData;

UCLASS()
class ELECTRICCASTLE_API AElectricCastleAIController : public AAIController
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AElectricCastleAIController();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InitializeDependencies();

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	UFUNCTION(BlueprintNativeEvent)
	void HandleAlertLevelChanged(const FAlertLevelChangePayload& Payload);
	UFUNCTION(BlueprintNativeEvent)
	void HandleAlertTargetPerceiveChanged(const FAlertTargetPerceivedChangePayload& Payload);
	UFUNCTION(BlueprintNativeEvent)
	void HandleEngagementLevelChanged(const FEngagementLevelChangedPayload& Payload);
	UFUNCTION(BlueprintNativeEvent)
	void HandleEngagementRangeChanged(const FEngagementRangeChangedPayload& Payload);
	UFUNCTION(BlueprintNativeEvent)
	void HandleEngagementTargetChanged(AActor* NewTarget);
	UFUNCTION(BlueprintNativeEvent)
	void HandlePawnDeath(AActor* DeadActor);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCinematicHandlerComponent> CinematicHandlerComponent;
	UPROPERTY()
	TObjectPtr<URandomRange> AttackRatePtr;
	UPROPERTY()
	TObjectPtr<URandomRange> PreferredDistancePtr;

private:
	TUniquePtr<FBBKeyCachedAccessor<UBlackboardKeyType_Enum>> AlertLevel;
	TUniquePtr<FBBKeyCachedAccessor<UBlackboardKeyType_Vector>> AlertLastKnownLocation;
	TUniquePtr<FBBKeyCachedAccessor<UBlackboardKeyType_Bool>> AlertTargetPerceived;
	TUniquePtr<FBBKeyCachedAccessor<UBlackboardKeyType_Enum>> EngagementControlMode;
	TUniquePtr<FBBKeyCachedAccessor<UBlackboardKeyType_Enum>> EngagementLevel;
	TUniquePtr<FBBKeyCachedAccessor<UBlackboardKeyType_Object>> EngagementAttackRate;
	TUniquePtr<FBBKeyCachedAccessor<UBlackboardKeyType_Enum>> EngagementRange;
	TUniquePtr<FBBKeyCachedAccessor<UBlackboardKeyType_Object>> EngagementPreferredDistance;
	TUniquePtr<FBBKeyCachedAccessor<UBlackboardKeyType_Object>> EngagementTarget;
	TUniquePtr<FBBKeyCachedAccessor<UBlackboardKeyType_Bool>> PatrolCanPatrol;
	TUniquePtr<FBBKeyCachedAccessor<UBlackboardKeyType_Vector>> PatrolNextPatrolPoint;
	TUniquePtr<FBBKeyCachedAccessor<UBlackboardKeyType_Float>> PatrolMovementSpeed;
	TUniquePtr<FBBKeyCachedAccessor<UBlackboardKeyType_Float>> PatrolWaitTime;
	TUniquePtr<FBBKeyCachedAccessor<UBlackboardKeyType_Bool>> StatusIsDead;

	void UpdateAttackRate(const FFloatRange& InAttackRate);
	void UpdatePreferredDistance(const FFloatRange& InPreferredDistance);
};
