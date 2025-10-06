// Copyright Alien Shores


#include "AbilitySystem/Calculations/ExecCalc_Damage.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "AbilitySystem/ElectricCastleAttributeSet.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Tags/ElectricCastleGameplayTags.h"

struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MagicPower);
	DECLARE_ATTRIBUTE_CAPTUREDEF(HitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Defense);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MagicDefense);
	DECLARE_ATTRIBUTE_CAPTUREDEF(EvadeChance);

	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;

	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UElectricCastleAttributeSet, AttackPower, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UElectricCastleAttributeSet, MagicPower, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UElectricCastleAttributeSet, HitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UElectricCastleAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UElectricCastleAttributeSet, Defense, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UElectricCastleAttributeSet, MagicDefense, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UElectricCastleAttributeSet, EvadeChance, Target, false);

		const FElectricCastleGameplayTags& Tags = FElectricCastleGameplayTags::Get();

		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_AttackPower, AttackPowerDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_MagicPower, MagicPowerDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_Defense, DefenseDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_MagicDefense, MagicDefenseDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_HitChance, HitChanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_EvadeChance, EvadeChanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitChance, CriticalHitChanceDef);
	}
};

static const AuraDamageStatics& DamageStatics()
{
	// static singleton 
	static AuraDamageStatics DStatics;
	return DStatics;
};

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().AttackPowerDef);
	RelevantAttributesToCapture.Add(DamageStatics().MagicPowerDef);
	RelevantAttributesToCapture.Add(DamageStatics().DefenseDef);
	RelevantAttributesToCapture.Add(DamageStatics().MagicDefenseDef);
	RelevantAttributesToCapture.Add(DamageStatics().HitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().EvadeChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
}

float UExecCalc_Damage::CalculateBaseDamage(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	const FAggregatorEvaluateParameters& EvaluateParameters
)
{
	FGameplayEffectContextHandle EffectContextHandle;
	float PhysicalDamage = 0.f;
	float MagicalDamage = 0.f;
	FGameplayTagContainer AbilityTags;
	ExecutionParams.GetOwningSpec().GetAllAssetTags(AbilityTags);
	if (IsPhysicalAttack(ExecutionParams))
	{
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
			DamageStatics().AttackPowerDef,
			EvaluateParameters,
			PhysicalDamage
		);
	}
	if (IsMagicalAttack(ExecutionParams))
	{
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
			DamageStatics().AttackPowerDef,
			EvaluateParameters,
			MagicalDamage
		);
	}
	const float Damage = PhysicalDamage > MagicalDamage ? PhysicalDamage : MagicalDamage;
	return Damage;
}

void UExecCalc_Damage::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput
) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	// Get Damage Set by Caller Magnitude
	float Damage = CalculateBaseDamage(ExecutionParams, EvaluateParameters);

	// If the attack was blocked (based on BlockChance), cut the damage in half.
	if (IsAttackEvadedByTarget(ExecutionParams, EvaluateParameters))
	{
		Damage = 0.f;
		UElectricCastleAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, true);
	}
	else
	{
		// Debuff
		DetermineDebuff(ExecutionParams, EvaluateParameters);
		if (IsRadialDamage(ExecutionParams))
		{
			ApplyRadialDamage(ExecutionParams, Damage);
		}
		// Reduce damage by a percentage based on target's effective armor and level of protection
		Damage = FMath::Clamp(Damage - GetTargetEffectiveArmor(ExecutionParams, EvaluateParameters), 0.f, Damage);
		// if the attack is a critical hit, increase the damage by the critical hit damage
		if (IsCriticalHitOnTarget(ExecutionParams, EvaluateParameters))
		{
			Damage *= 2.f;
			UElectricCastleAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, true);
		}
	}

	const FGameplayModifierEvaluatedData EvaluatedData(
		UElectricCastleAttributeSet::GetMeta_IncomingDamageAttribute(),
		EGameplayModOp::Additive,
		Damage
	);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}

bool UExecCalc_Damage::IsAttackEvadedByTarget(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	const FAggregatorEvaluateParameters& EvaluateParameters
)
{
	float EvadeChance = 0.f;
	float HitChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		DamageStatics().EvadeChanceDef,
		EvaluateParameters,
		EvadeChance
	);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		DamageStatics().HitChanceDef,
		EvaluateParameters,
		HitChance
	);
	const float EvadeCalc = FMath::RandRange(1, 100);
	return EvadeCalc >= (HitChance - EvadeChance);
}

float UExecCalc_Damage::GetTargetEffectiveArmor(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	const FAggregatorEvaluateParameters& EvaluateParameters
)
{
	float Defense = 0.f;
	float MagicDefense = 0.f;
	if (IsPhysicalAttack(ExecutionParams))
	{
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
			DamageStatics().DefenseDef,
			EvaluateParameters,
			Defense
		);
	}
	if (IsMagicalAttack(ExecutionParams))
	{
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
			DamageStatics().MagicDefenseDef,
			EvaluateParameters,
			MagicDefense
		);
	}
	return Defense > MagicDefense ? Defense : MagicDefense;
}

bool UExecCalc_Damage::IsCriticalHitOnTarget(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	const FAggregatorEvaluateParameters& EvaluateParameters
)
{
	float SourceCriticalHitChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		DamageStatics().CriticalHitChanceDef,
		EvaluateParameters,
		SourceCriticalHitChance
	);
	SourceCriticalHitChance = FMath::Max<float>(SourceCriticalHitChance, 0.f);
	const float CriticalHitCalculation = FMath::RandRange(1, 100);
	return CriticalHitCalculation <= SourceCriticalHitChance;
}

bool UExecCalc_Damage::IsDebuffApplied(
	const FGameplayTag& DamageTypeTag,
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	const FAggregatorEvaluateParameters& EvaluateParameters
)
{
	const FElectricCastleGameplayTags GameplayTags = FElectricCastleGameplayTags::Get();
	const FGameplayEffectSpec Spec = ExecutionParams.GetOwningSpec();
	if (Spec.GetSetByCallerMagnitude(DamageTypeTag, false, -1.f) > 0.f)
	{
		const float SourceDebuffChance = Spec.GetSetByCallerMagnitude(GameplayTags.Effect_Debuff_Stat_Chance, false, -1.f);
		// Create a random chance to resist a debuff
		const float TargetDebuffResistance = FMath::FRandRange(50.f, 100.f);
		const float EffectiveDebuffChance = SourceDebuffChance * (100 - TargetDebuffResistance) / 100.f;
		return FMath::RandRange(1, 100) < EffectiveDebuffChance;
	}
	return false;
}

void UExecCalc_Damage::DetermineDebuff(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	const FAggregatorEvaluateParameters& EvaluateParams
)
{
	const FElectricCastleGameplayTags& GameplayTags = FElectricCastleGameplayTags::Get();
	const FGameplayEffectSpec Spec = ExecutionParams.GetOwningSpec();

	for (const FGameplayTag& DebuffTag : FElectricCastleGameplayTags::Get().GetDebuffTypes())
	{
		if (IsDebuffApplied(DebuffTag, ExecutionParams, EvaluateParams))
		{
			FGameplayEffectContextHandle EffectContext = Spec.GetContext();
			UElectricCastleAbilitySystemLibrary::SetDebuff(
				EffectContext,
				DebuffTag,
				Spec.GetLevel(),
				Spec.GetSetByCallerMagnitude(GameplayTags.Effect_Debuff_Stat_Damage, false, -1.f),
				Spec.GetSetByCallerMagnitude(GameplayTags.Effect_Debuff_Stat_Duration, false, -1.f), Spec.GetSetByCallerMagnitude(GameplayTags.Effect_Debuff_Stat_Frequency, false, -1.f)
			);
			break;
		}
	}
}

bool UExecCalc_Damage::IsRadialDamage(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams
)
{
	return UElectricCastleAbilitySystemLibrary::IsRadialDamage(ExecutionParams.GetOwningSpec().GetContext());
}

void UExecCalc_Damage::ApplyRadialDamage(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	float& OutRadialDamage
)
{
	const FGameplayEffectSpec Spec = ExecutionParams.GetOwningSpec();
	const UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	AActor* TargetActor = TargetAbilitySystemComponent
		                      ? TargetAbilitySystemComponent->GetAvatarActor()
		                      : nullptr;
	const UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();
	AActor* SourceActor = SourceAbilitySystemComponent
		                      ? SourceAbilitySystemComponent->GetAvatarActor()
		                      : nullptr;
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(TargetActor))
	{
		FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
		CombatInterface->GetOnDamageDelegate().AddLambda(
			[&](float DamageAmount)
			{
				OutRadialDamage = DamageAmount;
			}
		);
		UGameplayStatics::ApplyRadialDamageWithFalloff(
			TargetActor,
			OutRadialDamage,
			0.f,
			UElectricCastleAbilitySystemLibrary::GetRadialDamageOrigin(EffectContextHandle),
			UElectricCastleAbilitySystemLibrary::GetRadialDamageInnerRadius(EffectContextHandle),
			UElectricCastleAbilitySystemLibrary::GetRadialDamageOuterRadius(EffectContextHandle),
			1.f,
			UDamageType::StaticClass(),
			TArray<AActor*>(),
			SourceActor,
			nullptr
		);
	}
}

bool UExecCalc_Damage::IsPhysicalAttack(const FGameplayEffectCustomExecutionParameters& ExecutionParams)
{
	FGameplayTagContainer AbilityTags;
	ExecutionParams.GetOwningSpec().GetAllAssetTags(AbilityTags);
	if (AbilityTags.HasTagExact(FElectricCastleGameplayTags::Get().Effect_Damage_Physical))
	{
		return true;
	}
	return false;
}

bool UExecCalc_Damage::IsMagicalAttack(const FGameplayEffectCustomExecutionParameters& ExecutionParams)
{
	FGameplayTagContainer AbilityTags;
	ExecutionParams.GetOwningSpec().GetAllAssetTags(AbilityTags);
	if (AbilityTags.HasTag(FElectricCastleGameplayTags::Get().Effect_Damage_Magic))
	{
		return true;
	}
	return false;
}
