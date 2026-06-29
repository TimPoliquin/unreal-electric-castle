// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Alert/AlertTypes.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Enum.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Float.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Engagement/EngagementTypes.h"
#include "Perception/AIPerceptionActor.h"
#include "Perception/AIPerceptionDataTypes.h"
#include "Targeting/AITargetingTypes.h"
#include "Utils/RandomRange.h"
#include "ElectricCastleAIController.generated.h"

class UAIPerceptionManager;
class UCinematicHandlerComponent;
class UBehaviorTreeComponent;
class UBlackboardComponent;
class UBlackboardData;

UCLASS()
class ELECTRICCASTLE_API AElectricCastleAIController : public AAIController, public IAIPerceptionActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AElectricCastleAIController();

	/** Start IAIPerceptionActor **/
	virtual UAIPerceptionComponent* GetAIPerceptionComponent_Implementation() const override { return PerceptionComponent; }
	/** End IAIPerceptionActor **/

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	void InitializeBlackboardKeys(UBlackboardData* BlackboardData);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InitializeDependencies();

	UFUNCTION(BlueprintNativeEvent)
	void HandleAlertLevelChanged(const FAlertLevelChangePayload& Payload);
	UFUNCTION(BlueprintNativeEvent)
	void HandleEngagementLevelChanged(const FEngagementLevelChangedPayload& Payload);
	UFUNCTION(BlueprintNativeEvent)
	void HandleEngagementRangeChanged(const FEngagementRangeChangedPayload& Payload);
	UFUNCTION()
	void HandleTargetChanged(const FAITargetChangedPayload& Payload);
	UFUNCTION(BlueprintNativeEvent)
	void HandlePawnDeath(AActor* DeadActor);
	UFUNCTION(BlueprintNativeEvent)
	void HandleEffectBlockAI(FGameplayTag EffectTag, int Count);
	UFUNCTION(BlueprintNativeEvent)
	void HandleEffectBlockMovement(FGameplayTag EffectTag, int Count);
	UFUNCTION(BlueprintNativeEvent)
	void HandleSignificanceLevelChanged(FGameplayTag SignificanceTag, int Count);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCinematicHandlerComponent> CinematicHandlerComponent;
	UPROPERTY()
	TObjectPtr<URandomRange> AttackRatePtr;
	UPROPERTY()
	TObjectPtr<URandomRange> PreferredDistancePtr;

private:
	TUniquePtr<FBBKeyCachedAccessor<UBlackboardKeyType_Enum>> AlertLevel;
	TUniquePtr<FBBKeyCachedAccessor<UBlackboardKeyType_Vector>> AlertLastKnownLocation;
	TUniquePtr<FBBKeyCachedAccessor<UBlackboardKeyType_Bool>> EffectBlockAI;
	TUniquePtr<FBBKeyCachedAccessor<UBlackboardKeyType_Bool>> EffectBlockAbilities;
	TUniquePtr<FBBKeyCachedAccessor<UBlackboardKeyType_Bool>> EffectBlockMovement;
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
	TUniquePtr<FBBKeyCachedAccessor<UBlackboardKeyType_Enum>> SignificanceLevel;

	void UpdateAttackRate(const FFloatRange& InAttackRate);
	void UpdatePreferredDistance(const FFloatRange& InPreferredDistance);
};
