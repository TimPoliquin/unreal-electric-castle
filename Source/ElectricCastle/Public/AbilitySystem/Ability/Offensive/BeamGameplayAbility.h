// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "DamageGameplayAbility.h"
#include "Actor/Beam/BeamActor.h"
#include "BeamGameplayAbility.generated.h"

class ABeamActor;
/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API UBeamGameplayAbility : public UDamageGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	) override;

protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PlayAbilitySoundCue();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PlayAbilityMontage();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SpawnBeam();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetCascadeTargetsCount() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnInputRelease(float TimeHeld);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Properties")
	bool bIsWeaponAbility = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Properties")
	FGameplayTag SocketTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties")
	TSubclassOf<ABeamActor> BeamClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties")
	bool bOverrideBeamProperties = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties", meta=(EditCondition="bOverrideBeamProperties", EditConditionHides))
	EBeamCascadeType BeamCascadeType = EBeamCascadeType::None;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties", meta=(EditCondition="bOverrideBeamProperties", EditConditionHides))
	FScalableFloat MaxBeamLength;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties", meta=(EditCondition="bOverrideBeamProperties", EditConditionHides))
	FScalableFloat BeamRadius;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties", meta=(EditCondition="bOverrideBeamProperties", EditConditionHides))
	FScalableFloat MaxCascades;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties", meta=(EditCondition="bOverrideBeamProperties", EditConditionHides))
	FScalableFloat CascadeDistance;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties", meta=(EditCondition="bOverrideBeamProperties", EditConditionHides))
	TEnumAsByte<ECollisionChannel> BeamTraceChannel = ECC_Visibility;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties")
	float MinimumSpellTime = .2f;

private:
	UPROPERTY()
	FTimerHandle DelayTimerHandle;
	UPROPERTY()
	TObjectPtr<ABeamActor> BeamActor;

	UFUNCTION()
	void OnDelayedRelease();
};
