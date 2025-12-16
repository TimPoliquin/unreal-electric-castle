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

protected:
	virtual FActiveGameplayEffectHandle ApplyBeamTargetEffect_Implementation(AActor* Target) override;
	virtual void InitializeChildBeamProperties_Implementation(ABeamActor* ChildBeam, const FHitResult& HitResult) override;

	UPROPERTY(BlueprintReadOnly, Replicated)
	FDamageEffectParams DamageEffectParams;
};
