// Copyright Alien Shores


#include "AbilitySystem/Effect/DurationGameplayEffect.h"

UDurationGameplayEffect::UDurationGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	Period = NO_PERIOD;
}
