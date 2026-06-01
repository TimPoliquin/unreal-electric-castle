// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "StatusEffectPlugin.h"
#include "Actor/Status/StatusEffectTypes.h"
#include "StatusEffectNiagaraPlugin.generated.h"

class UNiagaraSystem;
/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UStatusEffectNiagaraPlugin : public UStatusEffectPlugin
{
	GENERATED_BODY()

public:
	virtual void ApplyEffect_Implementation(const FGameplayTag& StatusEffectTag, AActor* InActor, const FStatusEffectDuration& InDuration) const override;
	virtual void RemoveEffect_Implementation(const FGameplayTag& StatusEffectTag, AActor* InActor) const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraSystem> NiagaraSystem;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FStatusEffectNiagaraConfig Config;
};
