// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/ElectricCastleGameplayAbility.h"
#include "AbilitySystem/ElectricCastleAbilitySystemTypes.h"
#include "DamageGameplayAbility.generated.h"

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

	UFUNCTION(BlueprintCallable)
	void DealDamage(AActor* TargetActor);

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

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void FaceTarget();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetFaceTargetLocation() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FGameplayCueParameters MakeGameplayCueParamsFromMontageTag(const FGameplayTag& MontageTag, const FVector& ImpactLocation) const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FGameplayCueParameters MakeDefaultGameplayCueParams(const FVector& ImpactLocation) const;

	UFUNCTION(BlueprintCallable)
	FVector GetTargetsAtImpact(const FGameplayTag& MontageTag, float ImpactRadius, TArray<AActor*>& OutTargets, bool bDebug = false) const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	FElectricCastleDamageConfig DamageConfig;

	int32 GetDamageAtLevel(const int32 AbilityLevel) const;

	UPROPERTY(EditDefaultsOnly, Category = "Configuration")
	bool bAutoActivateAbilityTag = true;
	UPROPERTY(EditDefaultsOnly, Category = "Configuration")
	bool bAutoDeactivateAbilityTag = true;

	FGameplayTag GetDefaultAbilityTag() const;
};
