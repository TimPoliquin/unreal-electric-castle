// Copyright Alien Shores


#include "AbilitySystem/Effect/InfiniteGameplayEffect.h"

UInfiniteGameplayEffect::UInfiniteGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	Period = NO_PERIOD;
}
