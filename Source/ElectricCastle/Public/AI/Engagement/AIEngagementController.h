// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "EngagementTypes.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "AIEngagementController.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API UAIEngagementController : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAIEngagementController();

	virtual void BeginPlay() override;
	virtual void Activate(bool bReset = false) override;
	virtual void Deactivate() override;

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
	void SetEngagementTarget(AActor* InTarget);
	UFUNCTION(BlueprintCallable)
	void RandomizeEngagement();
	UFUNCTION(BlueprintCallable)
	AActor* ChooseNewRandomTarget();
	UFUNCTION(BlueprintCallable)
	FGameplayTag ChooseAbility() const;

	UPROPERTY(BlueprintAssignable)
	FEngagementLevelChangedSignature OnEngagementLevelChanged;
	UPROPERTY(BlueprintAssignable)
	FEngagementRangeChangedSignature OnEngagementRangeChanged;
	UPROPERTY(BlueprintAssignable)
	FEngagementTargetChangedSignature OnEngagementTargetChanged;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FEngagementLevelConfig> EngagementLevelConfigs;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FEngagementRangeConfig> EngagementRangeConfigs;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FEngagementAbilityModeConfig> EngagementAbilityModeConfigs;

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
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	TWeakObjectPtr<AActor> Target;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDebug = false;

private:
	FEngagementLevelConfig GetEngagementLevelConfigByEngagementLevel(const EEngagementLevel InEngagementLevel) const;
	FEngagementRangeConfig GetEngagementRangeConfigByEngagementRange(const EEngagementRange InEngagementRange) const;

	FTimerHandle RandomizedEngagementTimerHandle;
};
