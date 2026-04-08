// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ExecCalc_Damage.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UExecCalc_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UExecCalc_Damage();

	virtual void Execute_Implementation(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput
	) const override;

protected:
	/** The Armor amount required to halve incoming damage */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DefenseRatioConstant = 10.f;

private:
	static float CalculateBaseDamage(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		const FAggregatorEvaluateParameters& EvaluateParameters
	);

	static bool IsAttackEvadedByTarget(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		const FAggregatorEvaluateParameters& EvaluateParameters
	);

	static bool IsAttackBlockedByTarget(const FGameplayEffectCustomExecutionParameters& ExecutionParams, const FAggregatorEvaluateParameters& EvaluateParameters);

	static bool IsAttackParriedByTarget(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		const FAggregatorEvaluateParameters& EvaluateParameters
	);

	static float GetTargetEffectiveArmor(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		const FAggregatorEvaluateParameters& EvaluateParameters
	);

	static bool IsCriticalHitOnTarget(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		const FAggregatorEvaluateParameters& EvaluateParameters
	);

	static bool IsDebuffApplied(
		const FGameplayTag& DamageTypeTag,
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		const FAggregatorEvaluateParameters& EvaluateParameters
	);

	static void DetermineDebuff(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		const FAggregatorEvaluateParameters& EvaluateParams
	);

	static bool IsRadialDamage(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams
	);

	static void ApplyRadialDamage(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		float& OutRadialDamage
	);

	static bool IsPhysicalAttack(const FGameplayEffectCustomExecutionParameters& ExecutionParams);
	static bool IsMagicalAttack(const FGameplayEffectCustomExecutionParameters& ExecutionParams);
};
