// Copyright Alien Shores


#include "Actor/Cinematic/Actions/DisableOwnerTickCinematicEventAction.h"

void UDisableOwnerTickCinematicEventAction::GetActorsToDisableTick_Implementation(TArray<AActor*>& OutActors) const
{
	OutActors.AddUnique(GetOwner());
}
