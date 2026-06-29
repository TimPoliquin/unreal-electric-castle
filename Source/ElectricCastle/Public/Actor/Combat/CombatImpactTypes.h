#pragma once

#include "CoreMinimal.h"
#include "CombatImpactTypes.generated.h"

class UNiagaraSystem;

USTRUCT(BlueprintType)
struct FCombatImpactConfig
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USoundBase> ImpactSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraSystem> ImpactEffect;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> ImpactMontage;
};
