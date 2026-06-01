// Copyright Alien Shores


#include "Actor/Status/StatusEffectActor.h"


UStatusEffectManagerComponent* IStatusEffectActor::GetStatusEffectManagerComponent(const UObject* Actor)
{
	return IsValid(Actor) && Actor->Implements<UStatusEffectActor>() ? Execute_GetStatusEffectManagerComponent(Actor) : nullptr;
}
