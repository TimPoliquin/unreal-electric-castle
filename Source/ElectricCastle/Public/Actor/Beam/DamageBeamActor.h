// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "BeamActor.h"
#include "AbilitySystem/ElectricCastleAbilitySystemTypes.h"
#include "DamageBeamActor.generated.h"

UCLASS(Abstract)
class ELECTRICCASTLE_API ADamageBeamActor : public ABeamActor
{
	GENERATED_BODY()

public:
	ADamageBeamActor();
	void SetDamageEffectParams(const FDamageEffectParams& InParams);
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Terminate_Implementation() override;

protected:
	virtual FActiveGameplayEffectHandle ApplyBeamTargetEffect_Implementation(AActor* Target) override;
	virtual void RemoveBeamTargetEffect_Implementation(AActor* Target, FActiveGameplayEffectHandle& Handle) override;
	virtual void InitializeChildBeamProperties_Implementation(ABeamActor* ChildBeam, const FHitResult& HitResult) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties")
	bool DealDamagePeriodically = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties", meta=(EditCondition="DealDamagePeriodically", EditConditionHides))
	float DamagePeriod = 0.1f;
	UPROPERTY(BlueprintReadOnly, Replicated)
	FDamageEffectParams DamageEffectParams;

private:
	UPROPERTY()
	TMap<TObjectPtr<AActor>, FTimerHandle> DamageEffectTimerHandles;
	void ApplyDamageEffect(AActor* Target) const;
};
