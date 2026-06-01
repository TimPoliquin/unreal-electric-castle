// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "ElectricCastleCharacter.h"
#include "EnemyInterface.h"
#include "AbilitySystem/AttributeChangeDelegates.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Actor/Spawn/TrackableInterface.h"
#include "AI/ElectricCastleAIController.h"
#include "AI/Alert/AIAlertActor.h"
#include "AI/Behavior/AIBehaviorTreeActor.h"
#include "AI/Engagement/AIEngagementActor.h"
#include "AI/Perception/AIPerceptionActor.h"
#include "Actor/Attack/Component/AttackWindowManagerActor.h"
#include "Actor/Patrol/PatrollingActor.h"
#include "Actor/Significance/SignificanceSensitiveActor.h"
#include "Components/TimelineComponent.h"
#include "ElectricCastleEnemyCharacter.generated.h"

class UActorSignificanceComponent;
class UAIAlertComponent;
class ULootSpawnComponent;
class UTimelineComponent;
class UMotionWarpingComponent;
class UBehaviorTree;
class AElectricCastleAIController;
class UWidgetComponent;

UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API AElectricCastleEnemyCharacter : public AElectricCastleCharacter,
                                                         public IEnemyInterface,
                                                         public ITrackableInterface,
                                                         public IAttackWindowManagerActor,
                                                         public IAIBehaviorTreeActor,
                                                         public IAIPerceptionActor,
                                                         public IAIAlertActor,
                                                         public IAIEngagementActor,
                                                         public IPatrollingActor,
                                                         public ISignificanceSensitiveActor
{
	GENERATED_BODY()

public:
	AElectricCastleEnemyCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual UElectricCastleAttributeSet* GetAttributeSet() const override { return AttributeSet; }
	virtual void PostInitializeComponents() override;

	void SetTargetingRange(const float InTargetingRange) { TargetingRange = InTargetingRange; }
	void SetMaxAIProcessingRange(const float InMaxProcessingRange) { MaxAIProcessingRange = InMaxProcessingRange; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetVisible(bool bInVisible);

	/** Start IElectricCastleAbilitySystemInterface **/
	FORCEINLINE virtual int GetCharacterLevel_Implementation() const override
	{
		return Level;
	}

	/** End IElectricCastleAbilitySystemInterface **/

	/** Start ICombatInterface **/
	virtual int32 GetXPReward_Implementation() const override;
	virtual AActor* GetWeapon_Implementation() const override;
	virtual TArray<FName> GetTargetTagsToIgnore_Implementation() const override;
	virtual void UpdateFacingTarget_Implementation(const FVector& FacingTarget) override;
	virtual void Die() override;
	/** End ICombatInterface **/

	// IEnemyInterface
	virtual AActor* GetCombatTarget_Implementation() const override;
	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;

	virtual float GetMaxAIProcessingRange() const override
	{
		return MaxAIProcessingRange;
	}

	/** Start ITrackableInterface **/
	virtual FOnTrackableStopTrackingSignature& GetStopTrackingDelegate() override;
	/** End ITrackableInterface **/
	/** Start IAttackWindowActor **/
	virtual UAttackWindowManager* GetAttackWindowManager_Implementation() const override;
	/** End IAttackWindowActor **/

	/** Start IAIBehaviorTreeActor **/
	virtual UBehaviorTree* GetBehaviorTree_Implementation() const override;
	virtual bool ShouldAutoRunBehaviorTree_Implementation() const override;
	/** End IAIBehaviorTreeActor **/

	/** Start IAIPerceptionActor **/
	virtual UAIPerceptionComponent* GetAIPerceptionComponent_Implementation() const override
	{
		return PerceptionComponent;
	}

	/** End IAIPerceptionActor **/

	/** Start IAIAlertActor **/
	virtual UAIAlertComponent* GetAIAlertComponent_Implementation() const override
	{
		return AIAlertComponent;
	}

	/** End IAIAlertActor **/

	/** Start AIEngagementActor **/
	virtual UAIEngagementController* GetAIEngagementController_Implementation() const override
	{
		return AIEngagementController;
	}

	/** End AIEngagementActor **/

	/** Start IPatrollingActor **/
	virtual UPatrolComponent* GetPatrolComponent_Implementation() const override
	{
		return PatrolComponent;
	}

	/** End IPatrollingActor **/

	/** Start ISignificanceSensitiveActor **/
	virtual void EnterSignificance_FullySignificant_Implementation() override;
	virtual void EnterSignificance_PartiallySignificant_Implementation() override;
	virtual void EnterSignificance_Insignificant_Implementation() override;
	/** End ISignificanceSensitiveActor **/

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnHealthChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnMaxHealthChanged;

	void SetLevel(const int32 InLevel) { Level = InLevel; }

protected:
	virtual void BeginPlay() override;
	virtual void InitializeAbilityActorInfo() override;
	virtual void InitializeDefaultAttributes();
	UFUNCTION(BlueprintNativeEvent)
	void HandleGameDataLoaded();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Spawn")
	void SpawnAnimation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Spawn")
	void SpawnAnimation_Initialize();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Spawn")
	void SpawnAnimation_Tick(const float Alpha);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Spawn")
	void SpawnAnimation_Finalize();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Loot")
	void SpawnLoot();
	UFUNCTION(BlueprintNativeEvent, Category="Socket Manager")
	void RegisterSockets(USocketManagerComponent* InSocketManagerComponent);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Stagger")
	UAnimMontage* GetStaggerMontage() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	bool bShouldAnimateSpawn = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn", meta=(EditCondition="bShouldAnimateSpawn", EditConditionHides=true))
	TObjectPtr<UCurveFloat> SpawnAnimationCurve;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UAIPerceptionComponent> PerceptionComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UAIAlertComponent> AIAlertComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UAIEngagementController> AIEngagementController;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UPatrolComponent> PatrolComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<ULootSpawnComponent> LootSpawnComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UChildActorComponent> WeaponChildActorComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UAttackWindowManager> AttackWindowManager;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UActorSignificanceComponent> SignificanceComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 Level = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character Class Defaults")
	TObjectPtr<UElectricCastleAttributeSet> AttributeSet;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UWidgetComponent> HealthWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float LifeSpan = 5.f;

	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	/** Attack range for the enemy. Note: This should probably be in the ability instead of the enemy. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float MaxAIProcessingRange = 2000;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float AttackRange = 500.f;
	/**
	 * Attack Range Tolerance
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float AttackRangeTolerance = 20.f;
	/** Targeting range for enemy. If the enemy detects a player within this range, it will move to attack. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float TargetingRange = 4000.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float AttackWaitTime = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float AttackWaitDeviation = .5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties")
	bool bDebug = false;

private:
	FOnTrackableStopTrackingSignature OnTrackableStopTracking;

	UPROPERTY(EditAnywhere, Category = "Highlight")
	uint8 HighlightCustomDepthStencilValue = 250;

	void InitializeAttributeDelegates();
	void InitializeStartupAbilities();
	UPROPERTY()
	TObjectPtr<UTimelineComponent> SpawnTimelineComponent;
	FOnTimelineFloat OnSpawnTimelineTick;
	FOnTimelineEvent OnSpawnTimelineFinished;
	FOnTimelineFloat OnDissolveTimelineTick;
	FOnTimelineEvent OnDissolveTimelineFinished;
	TArray<TFunction<void()>> OnReadyForAIController;
	FActiveGameplayEffectHandle SignificanceEffectHandle;
};
