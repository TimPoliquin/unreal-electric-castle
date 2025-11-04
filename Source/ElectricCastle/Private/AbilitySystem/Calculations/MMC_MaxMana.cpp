// Copyright Alien Shores


#include "AbilitySystem/Calculations/MMC_MaxMana.h"

// Copyright Alien Shores


#include "AbilitySystem/ElectricCastleAbilitySystemInterface.h"
#include "AbilitySystem/ElectricCastleAttributeSet.h"

UMMC_MaxMana::UMMC_MaxMana()
{
	IntelligenceDefinition.AttributeToCapture = UElectricCastleAttributeSet::GetIntelligenceAttribute();
	IntelligenceDefinition.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	IntelligenceDefinition.bSnapshot = false;
	RelevantAttributesToCapture.Add(IntelligenceDefinition);
}

float UMMC_MaxMana::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// Gather tags from source and target
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float Intelligence = 0.f;
	GetCapturedAttributeMagnitude(IntelligenceDefinition, Spec, EvaluateParameters, Intelligence);
	// Ensure Intelligence is always a positive number
	Intelligence = FMath::Max(Intelligence, 0.f);

	// Max Mana is a function of Intelligence and character level.
	const int32 PlayerLevel = IElectricCastleAbilitySystemInterface::GetCharacterLevel(Spec.GetContext().GetSourceObject());
	return 47.5f + (2.5f * Intelligence) + (15.f * (PlayerLevel - 1));
}
