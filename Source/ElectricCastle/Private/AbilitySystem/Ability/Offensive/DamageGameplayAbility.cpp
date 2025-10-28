// Copyright Alien Shores


#include "AbilitySystem/Ability/Offensive/DamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "Character/EnemyInterface.h"
#include "Interaction/CombatInterface.h"
#include "Tags/ElectricCastleGameplayTags.h"
#include "Utils/ArrayUtils.h"

void UDamageGameplayAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (bAutoActivateAbilityTag)
	{
		ICombatInterface::SetActiveAbilityTag(GetAvatarActorFromActorInfo(), GetDefaultAbilityTag());
	}
}

void UDamageGameplayAbility::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	if (bAutoDeactivateAbilityTag)
	{
		ICombatInterface::ClearActiveAbilityTag(GetAvatarActorFromActorInfo());
	}
}


void UDamageGameplayAbility::DealDamage(AActor* TargetActor)
{
	const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1);
	const int32 Level = GetAbilityLevel();
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle,
		DamageConfig.DamageTypeTag,
		DamageConfig.Amount.GetValueAtLevel(Level)
	);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(
		*SpecHandle.Data.Get(),
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor)
	);
}

void UDamageGameplayAbility::DamageTargets_Implementation(const TArray<AActor*>& Targets, const FVector& ImpactLocation, const FGameplayTag& MontageTag)
{
	for (AActor* Target : Targets)
	{
		const FDamageEffectParams DamageEffectParams = MakeDamageEffectParamsFromClassDefaults(Target, ImpactLocation);
		UElectricCastleAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		if (ImpactCueTag.IsValid())
		{
			const FGameplayCueParameters ImpactCueParams = MakeGameplayCueParamsFromMontageTag(MontageTag, ImpactLocation);
			K2_ExecuteGameplayCueWithParams(ImpactCueTag, ImpactCueParams);
		}
	}
}

FTaggedMontage UDamageGameplayAbility::GetRandomAttackMontage() const
{
	const AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (const TArray<FTaggedMontage> AttackMontages = ICombatInterface::GetAttackMontages(AvatarActor); AttackMontages.
		Num() > 0)
	{
		return UArrayUtils::GetRandomElement(AttackMontages);
	}
	UE_LOG(LogTemp, Warning, TEXT("No attack montage tags associated for %s"), *AvatarActor->GetName());
	return FTaggedMontage();
}

FDamageEffectParams UDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(
	AActor* TargetActor,
	FVector RadialDamageOrigin,
	bool bOverrideKnockbackDirection,
	FVector InKnockbackDirectionOverride,
	bool bOverrideDeathImpulse,
	FVector InDeathImpulseDirectionOverride,
	bool bOverridePitch,
	float PitchOverride
) const
{
	FGameplayTagContainer AssetTags;
	if (const UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo()))
	{
		const FGameplayTagContainer& ActiveTags = AbilitySystemComponent->GetOwnedGameplayTags();
		const FGameplayTagContainer& DamageTags = ActiveTags.Filter(FElectricCastleGameplayTags::Get().Effect_Damage.GetSingleTagContainer());
		AssetTags.AppendTags(GetAssetTags());
		AssetTags.AppendTags(DamageTags);
	} else
	{
		AssetTags = GetAssetTags();
	}
	return UElectricCastleAbilitySystemLibrary::MakeCustomDamageEffectParams(
		GetAvatarActorFromActorInfo(),
		TargetActor,
		DamageEffectClass,
		DamageConfig,
		GetAbilityLevel(),
		AssetTags,
		RadialDamageOrigin,
		bOverrideKnockbackDirection,
		InKnockbackDirectionOverride,
		bOverrideDeathImpulse,
		InDeathImpulseDirectionOverride,
		bOverridePitch,
		PitchOverride
	);
}

FVector UDamageGameplayAbility::GetFaceTargetLocation() const
{
	if (const AActor* Target = IEnemyInterface::GetCombatTarget(GetAvatarActorFromActorInfo()))
	{
		return Target->GetActorLocation();
	}
	return GetAvatarActorFromActorInfo()->GetActorLocation();
}

FGameplayCueParameters UDamageGameplayAbility::MakeDefaultGameplayCueParams(const FVector& ImpactLocation) const
{
	FGameplayCueParameters CueParams;
	CueParams.AbilityLevel = GetAbilityLevel();
	CueParams.GameplayEffectLevel = GetAbilityLevel();
	CueParams.EffectCauser = GetAvatarActorFromActorInfo();
	CueParams.Instigator = GetAvatarActorFromActorInfo();
	CueParams.SourceObject = IEnemyInterface::GetCombatTarget(GetAvatarActorFromActorInfo());
	CueParams.Location = ImpactLocation;
	CueParams.AggregatedSourceTags.AddTagFast(GetDefaultAbilityTag());
	return CueParams;
}

FGameplayCueParameters UDamageGameplayAbility::MakeGameplayCueParamsFromMontageTag(const FGameplayTag& MontageTag, const FVector& ImpactLocation) const
{
	FGameplayCueParameters CueParams = MakeDefaultGameplayCueParams(ImpactLocation);
	CueParams.AggregatedSourceTags.AddTag(MontageTag);
	return CueParams;
}

FVector UDamageGameplayAbility::GetTargetsAtImpact(const FGameplayTag& MontageTag, const float ImpactRadius, TArray<AActor*>& OutTargets, bool bDebug) const
{
	const FVector& SocketLocation = ICombatInterface::GetCombatSocketLocation(GetAvatarActorFromActorInfo(), MontageTag);
	const TArray<FName> IgnoreTargetTags = ICombatInterface::GetTargetTagsToIgnore(GetAvatarActorFromActorInfo());
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
	UElectricCastleAbilitySystemLibrary::GetLiveActorsWithinRadius(GetAvatarActorFromActorInfo(), ActorsToIgnore, IgnoreTargetTags, SocketLocation, ImpactRadius, OutTargets, bDebug);
	return SocketLocation;
}

void UDamageGameplayAbility::GetTargetsAtImpactLocation(const FVector& ImpactLocation, float ImpactRadius, TArray<AActor*>& OutTargets, bool bDebug) const
{
	const TArray<FName> IgnoreTargetTags = ICombatInterface::GetTargetTagsToIgnore(GetAvatarActorFromActorInfo());
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
	UElectricCastleAbilitySystemLibrary::GetLiveActorsWithinRadius(GetAvatarActorFromActorInfo(), ActorsToIgnore, IgnoreTargetTags, ImpactLocation, ImpactRadius, OutTargets, bDebug);
}

void UDamageGameplayAbility::FaceTarget_Implementation()
{
	if (const AActor* Target = IEnemyInterface::GetCombatTarget(GetAvatarActorFromActorInfo()))
	{
		ICombatInterface::UpdateFacingTarget(GetAvatarActorFromActorInfo(), Target->GetActorLocation());
	}
}

int32 UDamageGameplayAbility::GetDamageAtLevel(
	const int32 AbilityLevel
) const
{
	return DamageConfig.GetDamageAtLevel(AbilityLevel);
}

FGameplayTag UDamageGameplayAbility::GetDefaultAbilityTag() const
{
	return GetAssetTags().First();
}
