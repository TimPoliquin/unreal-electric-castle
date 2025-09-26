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

private:
	UPROPERTY(EditDefaultsOnly, Category="Beam")
	TObjectPtr<UAnimMontage> AbilityMontage;
	UPROPERTY(EditDefaultsOnly, Category="Beam", meta=(Categories = "Events.Montage"))
	FGameplayTag EventTag;
	UPROPERTY(EditDefaultsOnly, Category="Beam", meta=(Categories = "GameplayCue"))
	FGameplayTag SoundCueTag;
	UPROPERTY(EditDefaultsOnly, Category="Beam", meta=(Categories = "GameplayCue"))
	FGameplayTag LoopCueTag;
	UPROPERTY(EditDefaultsOnly, Category="Beam", meta=(Categories = "Combat.Socket"))
	FGameplayTag SocketTag;
	UPROPERTY(EditDefaultsOnly, Category="Beam")
	float BeamTraceSize = 10.f;
	UPROPERTY(EditDefaultsOnly, Category="Beam")
	float CascadeRadius = 100.f;
	UPROPERTY(EditDefaultsOnly, Category="Beam")
	int32 MaxCascadeTargets = 5.f;
	UPROPERTY(EditDefaultsOnly, Category="Beam")
	float DamageTick = .1f;
	UPROPERTY(EditDefaultsOnly, Category="Beam|Debug")
	bool bForceCascadeMax = false;
	UPROPERTY()
	FTimerHandle TimerHandle;
	UPROPERTY()
	FTimerHandle DelayTimerHandle;
	UPROPERTY(EditDefaultsOnly, Category="Beam")
	float MinimumSpellTime = .5f;

	FVector HitLocation;
	UPROPERTY()
	TObjectPtr<AActor> PrimaryTarget;
	AActor* TraceFirstTarget(const FVector& BeamTargetLocation);
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

	UFUNCTION()
	void SpawnBeam(FGameplayEventData Payload);
	void ExecuteAbility(const FHitResult& HitResult);
	void SetMouseCursorVisible(const bool Visible) const;
	void SetMovementEnabled(const bool Enabled) const;
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
