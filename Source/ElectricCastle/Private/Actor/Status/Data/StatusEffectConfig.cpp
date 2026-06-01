// Copyright Alien Shores


#include "Actor/Status/Data/StatusEffectConfig.h"

#include "Actor/Status/Plugin/StatusEffectPlugin.h"

void FStatusEffectRow::ApplyEffect(AActor* InActor, const FStatusEffectDuration& InDuration) const
{
	if (!InActor)
	{
		return;
	}
	for (const UStatusEffectPlugin* StatusEffectPlugin : Plugins)
	{
		if (!StatusEffectPlugin)
		{
			continue;
		}
		StatusEffectPlugin->ApplyEffect(EffectTag, InActor, InDuration);
	}
}

void FStatusEffectRow::RemoveEffect(AActor* InActor) const
{
	if (!InActor)
	{
		return;
	}
	for (const UStatusEffectPlugin* StatusEffectPlugin : Plugins)
	{
		if (!StatusEffectPlugin)
		{
			continue;
		}
		StatusEffectPlugin->RemoveEffect(EffectTag, InActor);
	}
}

FStatusEffectRow UStatusEffectConfig::FindStatusEffectRowByTags(const FGameplayTagContainer& EffectTags) const
{
	if (EffectTags.IsEmpty())
	{
		return FStatusEffectRow();
	}
	const FStatusEffectRow* Match = StatusEffects.FindByPredicate(
		[EffectTags](const FStatusEffectRow& StatusEffect)
		{
			return EffectTags.HasTagExact(StatusEffect.EffectTag);
		}
	);
	if (Match)
	{
		return *Match;
	}
	return FStatusEffectRow();
}

FGameplayTagContainer UStatusEffectConfig::GetAllStatusEffectTags() const
{
	FGameplayTagContainer StatusEffectTags;
	for (const FStatusEffectRow& StatusEffect : StatusEffects)
	{
		StatusEffectTags.AddTag(StatusEffect.EffectTag);
	}
	return StatusEffectTags;
}

FGameplayTagContainer UStatusEffectConfig::GetMatchingStatusEffectTags(const FGameplayTagContainer& Tags) const
{
	return Tags.FilterExact(GetAllStatusEffectTags());
}
