// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "EngagementTypes.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "AI/Targeting/AITargetingTypes.h"
#include "Components/ActorComponent.h"
#include "AIEngagementController.generated.h"


class AEncounterManager;
class UEngagementLevelConfig;
class UEngagementMovementPlugin;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API UAIEngagementController : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAIEngagementController();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void Activate(bool bReset = false) override;
	virtual void Deactivate() override;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ChangeItUp();
	UFUNCTION(BlueprintCallable)
	void TickMovement(const float DeltaTime) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	EEngagementControlMode GetCurrentEngagementControlMode() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	EEngagementLevel GetCurrentEngagementLevel() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FFloatRange GetCurrentEngagementAttackRate() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	EEngagementRange GetCurrentEngagementRange() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FFloatRange GetCurrentPreferredRange() const;

	UFUNCTION(BlueprintCallable)
	void SetCurrentEngagementControlMode(const EEngagementControlMode InEngagementControlMode);
	UFUNCTION(BlueprintCallable)
	void SetCurrentEngagementLevel(const EEngagementLevel InEngagementLevel);
	UFUNCTION(BlueprintCallable)
	void SetCurrentEngagementRange(const EEngagementRange InEngagementRange);
	UFUNCTION(BlueprintCallable)
	void SetCurrentEngagementAbilityModes(TArray<EEngagementAbilityMode> InEngagementAbilityModes);
	UFUNCTION(BlueprintCallable)
	void RandomizeEngagement();
	UFUNCTION(BlueprintCallable)
	AActor* ChooseNewRandomTarget();
	UFUNCTION(BlueprintCallable)
	FGameplayTag ChooseAbility() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool DoesPreferAggressiveEngagement() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	EEngagementRange GetPreferredEngagementRange() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	EEngagementRange GetRandomEngagementRange() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	EEngagementLevel GetRandomAggressiveEngagementLevel() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	EEngagementLevel GetRandomPassiveEngagementLevel(const bool bAllowNoneEngagement) const;

	UPROPERTY(BlueprintAssignable)
	FEngagementLevelChangedSignature OnEngagementLevelChanged;
	UPROPERTY(BlueprintAssignable)
	FEngagementRangeChangedSignature OnEngagementRangeChanged;

protected:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool ShouldUpdateMovement() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Export)
	TArray<TObjectPtr<UEngagementLevelConfig>> EngagementLevelConfigs;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FEngagementRangeConfig> EngagementRangeConfigs;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FEngagementAbilityModeConfig> EngagementAbilityModeConfigs;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer BlockMovementTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEngagementControlMode EngagementControlMode = EEngagementControlMode::Static;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="EngagementControlMode == EEngagementControlMode::Randomized", EditConditionHides))
	FFloatRange RandomizeTime = FFloatRange::Inclusive(5.f, 30.f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="EngagementControlMode == EEngagementControlMode::Static"))
	EEngagementLevel CurrentEngagementLevel = EEngagementLevel::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="EngagementControlMode == EEngagementControlMode::Static"))
	EEngagementRange CurrentEngagementRange = EEngagementRange::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="EngagementControlMode == EEngagementControlMode::Static"))
	TArray<EEngagementAbilityMode> CurrentEngagementAbilityModes = {EEngagementAbilityMode::None};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, meta=(EditCondition="EngagementControlMode == EEngagementControlMode::Static"))
	TObjectPtr<UEngagementMovementPlugin> CurrentMovementPlugin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDebug = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDrawVisualDebug = false;

private:
	UFUNCTION()
	void HandleOwnerDeath(AActor* DeadActor);
	UFUNCTION()
	void SetMovementPlugin(UEngagementMovementPlugin* InEngagementMovementPlugin);

	UEngagementLevelConfig* GetEngagementLevelConfigByEngagementLevel(const EEngagementLevel InEngagementLevel) const;
	FEngagementRangeConfig GetEngagementRangeConfigByEngagementRange(const EEngagementRange InEngagementRange) const;
};
