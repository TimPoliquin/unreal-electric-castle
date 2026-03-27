// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/ElectricCastleGameplayAbility.h"
#include "AbilitySystem/ElectricCastleAbilitySystemTypes.h"
#include "DamageGameplayAbility.generated.h"

class IDamageDealingActor;
struct FTaggedMontage;

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API UDamageGameplayAbility : public UElectricCastleGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;
	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void DamageTargets(const TArray<AActor*>& Targets, const FVector& ImpactLocation, const FGameplayTag& MontageTag);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void DamageTarget(AActor* Target, const FVector& ImpactLocation, const FGameplayTag& MontageTag);


	UFUNCTION(BlueprintCallable)
	FTaggedMontage GetRandomAttackMontage() const;

	UFUNCTION(BlueprintPure)
	FDamageEffectParams MakeDamageEffectParamsFromClassDefaults(
		AActor* TargetActor = nullptr,
		FVector RadialDamageOrigin = FVector::ZeroVector,
		bool bOverrideKnockbackDirection = false,
		FVector InKnockbackDirectionOverride = FVector::ZeroVector,
		bool bOverrideDeathImpulse = false,
		FVector InDeathImpulseDirectionOverride = FVector::ZeroVector,
		bool bOverridePitch = false,
		float PitchOverride = 0.f
	) const;
	UFUNCTION(BlueprintPure)
	FDamageEffectParams MakeDamageEffectParams() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void FaceTarget();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetFaceTargetLocation() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FGameplayCueParameters MakeGameplayCueParamsFromMontageTag(
		const FGameplayTag& MontageTag,
		const FVector& ImpactLocation
	) const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FGameplayCueParameters MakeDefaultGameplayCueParams(const FVector& ImpactLocation) const;

	UFUNCTION(BlueprintCallable)
	FVector GetTargetsAtImpact(
		const FGameplayTag& MontageTag,
		float ImpactRadius,
		TArray<AActor*>& OutTargets,
		bool bInDebug = false
	) const;
	UFUNCTION(BlueprintCallable)
	void GetTargetsAtImpactLocation(
		const FVector& ImpactLocation,
		float ImpactRadius,
		TArray<AActor*>& OutTargets,
		bool bInDebug = false
	) const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	void GetTargetsInAttackRange(
		const FVector& AttackStart,
		const FVector& AttackEnd,
		float ImpactRadius,
		TArray<AActor*>& OutTargets,
		bool bInDebug = false
	) const;

	UFUNCTION(BlueprintCallable)
	void ApplyDefaultDamageConfig(AActor* DamageDealingActor) const;
	UFUNCTION(BlueprintCallable)
	void SetOwnerAndInstigator(AActor* Actor) const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties|Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties|Damage")
	FElectricCastleDamageConfig DamageConfig;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Properties|Damage", meta=(Categories="GameplayCue"))
	FGameplayTag ImpactCueTag = FGameplayTag::EmptyTag;

	float GetDamageMagnitudeAtLevel(const int32 AbilityLevel) const;

	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	bool bAutoActivateAbilityTag = true;
	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	bool bAutoDeactivateAbilityTag = true;

	FGameplayTag GetDefaultAbilityTag() const;
};
