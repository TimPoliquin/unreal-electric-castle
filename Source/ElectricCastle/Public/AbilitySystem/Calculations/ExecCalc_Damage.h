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

private:
	static float CalculateBaseDamage(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		const FAggregatorEvaluateParameters& EvaluateParameters
	);

	static float GetDamageTypeDamage(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		const FAggregatorEvaluateParameters& EvaluateParameters,
		const FGameplayTag& DamageTypeTag
	);

	static bool IsAttackBlockedByTarget(
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

	static float GetEffectiveCriticalHitDamage(
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
};
