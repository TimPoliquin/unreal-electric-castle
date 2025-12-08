// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "DamageGameplayAbility.h"
#include "BeamGameplayAbility.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API UBeamGameplayAbility : public UDamageGameplayAbility
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

protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ExecuteAbility(const FHitResult& HitResult);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PlayAbilitySoundCue();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PlayAbilityMontage();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SpawnBeam(FGameplayEventData Payload);
	UFUNCTION(BlueprintCallable)
	void SetMouseCursorVisible(const bool Visible) const;
	UFUNCTION(BlueprintCallable)
	void SetMovementEnabled(const bool Enabled) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties|Beam")
	TObjectPtr<UAnimMontage> AbilityMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties|Beam", meta=(Categories = "Events.Montage"))
	FGameplayTag EventTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties|Beam", meta=(Categories = "GameplayCue"))
	FGameplayTag SoundCueTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties|Beam", meta=(Categories = "GameplayCue"))
	FGameplayTag LoopCueTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties|Beam", meta=(Categories = "Combat.Socket"))
	FGameplayTag SocketTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties|Beam")
	float MaxBeamLength = 5000.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties|Beam")
	float BeamTraceSize = 10.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties|Beam")
	float CascadeRadius = 100.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties|Beam")
	int32 MaxCascadeTargets = 5.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties|Beam")
	float DamageTick = .1f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties|Beam|Debug")
	bool bForceCascadeMax = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties|Beam")
	float MinimumSpellTime = .5f;

private:
	UPROPERTY()
	FTimerHandle TimerHandle;
	UPROPERTY()
	FTimerHandle DelayTimerHandle;

	UPROPERTY()
	FHitResult TraceHitResult;
	UPROPERTY()
	TObjectPtr<AActor> PrimaryTarget;
	UPROPERTY()
	TMap<AActor*, FGameplayCueParameters> ActorGameplayCueParameters;
	UPROPERTY()
	TArray<AActor*> CueActors;


	UFUNCTION()
	void OnDelayedRelease();
	UFUNCTION()
	void OnInputRelease(float TimeHeld);
	UFUNCTION()
	void OnReceiveMouseData(const FGameplayAbilityTargetDataHandle& DataHandle);

	AActor* DetermineCueTarget(AActor* ActorHit) const;
	void DetermineCascadingTargets(AActor* CueTarget, TArray<AActor*>& OutCascadedTargets);

	float GetCascadeRadius() const
	{
		return CascadeRadius;
	}

	int32 GetCascadeTargetsCount() const;

	void ApplyCueToTarget(AActor* CueTarget, const FGameplayCueParameters& Parameters);
	void CascadeToActor(const AActor* FromActor, AActor* CascadeTarget);

	UFUNCTION()
	void OnTimerTick();
	void InitializeTimer();
	void ApplyDamage(AActor* DamageActor);
	UFUNCTION()
	void OnPrimaryTargetDead(AActor* TargetActor);
	UFUNCTION()
	void OnCascadeTargetDead(AActor* CascadeTarget);
	void BindPrimaryTargetDeath(AActor* Actor);
	void BindCascadeTargetDeath(AActor* Actor);
	bool IsTargetALivingEnemy(const AActor* TargetActor) const;
	void EndAbilityOnTargets();
	UFUNCTION()
	void OnRecieveNoTarget(const FGameplayAbilityTargetDataHandle& DataHandle);
};
