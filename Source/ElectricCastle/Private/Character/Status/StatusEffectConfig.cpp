// Copyright Alien Shores


#include "Character/Status/StatusEffectConfig.h"

bool UStatusEffectConfig::FindStatusEffectByTag(const FGameplayTag& EffectTag, FStatusEffectRow& OutStatusEffect) const
{
	const FStatusEffectRow* Match = StatusEffects.FindByPredicate([EffectTag](const FStatusEffectRow& StatusEffect)
	{
		return StatusEffect.EffectTag.MatchesTagExact(EffectTag);
	});
	if (Match)
	{
		OutStatusEffect = *Match;
		return true;
	}
	return false;
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
