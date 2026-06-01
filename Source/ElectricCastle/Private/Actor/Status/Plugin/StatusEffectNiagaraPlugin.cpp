// Copyright Alien Shores


#include "Actor/Status/Plugin/StatusEffectNiagaraPlugin.h"

#include "NiagaraSystem.h"
#include "Actor/Status/StatusEffectActor.h"
#include "Actor/Status/StatusEffectManagerComponent.h"

void UStatusEffectNiagaraPlugin::ApplyEffect_Implementation(const FGameplayTag& StatusEffectTag, AActor* InActor, const FStatusEffectDuration& InDuration) const
{
	if (!IsValid(NiagaraSystem))
	{
		return;
	}
	if (UStatusEffectManagerComponent* StatusEffectManagerComponent = IStatusEffectActor::GetStatusEffectManagerComponent(InActor))
	{
		StatusEffectManagerComponent->AddStatusEffectNiagaraSystem(StatusEffectTag, NiagaraSystem, Config);
	}
}

void UStatusEffectNiagaraPlugin::RemoveEffect_Implementation(const FGameplayTag& StatusEffectTag, AActor* InActor) const
{
	if (!IsValid(NiagaraSystem))
	{
		return;
	}
	if (UStatusEffectManagerComponent* StatusEffectManagerComponent = IStatusEffectActor::GetStatusEffectManagerComponent(InActor))
	{
		StatusEffectManagerComponent->RemoveStatusEffectNiagaraSystem(StatusEffectTag);
	}
}
