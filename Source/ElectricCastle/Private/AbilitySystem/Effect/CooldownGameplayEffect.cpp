// Copyright Alien Shores


#include "CooldownGameplayEffect.h"

UCooldownGameplayEffect::UCooldownGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	Period = NO_PERIOD;
}
