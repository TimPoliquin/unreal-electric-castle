// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "ElectricCastleCharacter.h"
#include "EnemyInterface.h"
#include "Interaction/HighlightInterface.h"
#include "AbilitySystem/AttributeChangeDelegates.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Actor/Spawn/TrackableInterface.h"
#include "AI/ElectricCastleAIController.h"
#include "Components/TimelineComponent.h"
#include "ElectricCastleEnemyCharacter.generated.h"

class ULootSpawnComponent;
class UTimelineComponent;
class UMotionWarpingComponent;
class UBehaviorTree;
class AElectricCastleAIController;
class UWidgetComponent;

UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API AElectricCastleEnemyCharacter : public AElectricCastleCharacter, public IHighlightInterface, public IEnemyInterface, public ITrackableInterface
{
	GENERATED_BODY()

public:
	AElectricCastleEnemyCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual UElectricCastleAttributeSet* GetAttributeSet() const override { return AttributeSet; }

	virtual void OnHitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount) override;
	void SetTargetingRange(const float InTargetingRange) { TargetingRange = InTargetingRange; }
	void SetMaxAIProcessingRange(const float InMaxProcessingRange) { MaxAIProcessingRange = InMaxProcessingRange; }
	// IHighlightInterface
	virtual void HighlightActor_Implementation() override;
	virtual void UnHighlightActor_Implementation() override;

	virtual bool SetMoveToLocation_Implementation(FVector& OutDestination) override
	{
		// do not override destination
		return false;
	}


	/** Start IElectricCastleAbilitySystemInterface **/
	FORCEINLINE virtual int GetCharacterLevel_Implementation() const override
	{
		return Level;
	}

	/** End IElectricCastleAbilitySystemInterface **/

	/** Start ICombatInterface **/
	virtual int32 GetXPReward_Implementation() const override;
	virtual USkeletalMeshComponent* GetWeapon_Implementation() const override;
	virtual TArray<FName> GetTargetTagsToIgnore_Implementation() const override;
	virtual void UpdateFacingTarget_Implementation(const FVector& FacingTarget) override;
	virtual void Die() override;
	/** End ICombatInterface **/

	// IEnemyInterface
	FORCEINLINE virtual AActor* GetCombatTarget_Implementation() const override
	{
		return CombatTarget;
	}

	FORCEINLINE virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override
	{
		CombatTarget = InCombatTarget;
	}

	virtual float GetMaxAIProcessingRange() const override
	{
		return MaxAIProcessingRange;
	}

	/** Start ITrackableInterface **/
	virtual FOnTrackableStopTrackingSignature& GetStopTrackingDelegate() override;
	/** End ITrackableInterface **/

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnHealthChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnMaxHealthChanged;

	void SetLevel(const int32 InLevel) { Level = InLevel; }
	ECharacterClass GetCharacterClass() const;
	void SetCharacterClass(const ECharacterClass InCharacterClass);

protected:
	virtual void BeginPlay() override;
	virtual void InitializeAbilityActorInfo() override;
	virtual void InitializeDefaultAttributes();
	virtual void OnStatusShockAdded() override;
	virtual void OnStatusShockRemoved() override;
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Highlight")
	bool bHighlighted;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	bool bShouldAnimateSpawn = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn", meta=(EditCondition="bShouldAnimateSpawn", EditConditionHides=true))
	TObjectPtr<UCurveFloat> SpawnAnimationCurve;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<ULootSpawnComponent> LootSpawnComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 Level = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character Class Defaults")
	TObjectPtr<UElectricCastleAttributeSet> AttributeSet;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UWidgetComponent> HealthWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float LifeSpan = 5.f;

	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;
	UPROPERTY()
	TObjectPtr<AElectricCastleAIController> AuraAIController;
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

	UPROPERTY(BlueprintReadWrite, Category = "AI")
	TObjectPtr<AActor> CombatTarget;

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
};
