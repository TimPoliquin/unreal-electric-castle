// Copyright Alien Shores


#include "AbilitySystem/Calculations/MMC_MaxHealth.h"

#include "AbilitySystem/ElectricCastleAbilitySystemInterface.h"
#include "AbilitySystem/ElectricCastleAttributeSet.h"

UMMC_MaxHealth::UMMC_MaxHealth()
{
	VigorDefinition.AttributeToCapture = UElectricCastleAttributeSet::GetVigorAttribute();
	VigorDefinition.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	VigorDefinition.bSnapshot = false;
	RelevantAttributesToCapture.Add(VigorDefinition);
}

float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// Gather tags from source and target
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float Vigor = 0.f;
	GetCapturedAttributeMagnitude(VigorDefinition, Spec, EvaluateParameters, Vigor);
	// Ensure Vigor is always a positive number
	Vigor = FMath::Max(Vigor, 0.f);

	// Max health is a function of vigor and character level.
	const int32 PlayerLevel = IElectricCastleAbilitySystemInterface::GetCharacterLevel(Spec.GetContext().GetSourceObject());
	const float CalculatedMaxHealth = 77.5f + (2.5f * Vigor) + (10.f * (PlayerLevel - 1));
	return CalculatedMaxHealth;
}
