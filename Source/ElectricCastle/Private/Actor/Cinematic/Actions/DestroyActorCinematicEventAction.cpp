// Copyright Alien Shores


#include "Actor/Cinematic/Actions/DestroyActorCinematicEventAction.h"

#include "Tags/ElectricCastleGameplayTags.h"

UDestroyActorCinematicEventAction::UDestroyActorCinematicEventAction()
{
	Triggers.AddTag(FElectricCastleGameplayTags::Get().Cinematic_Reaction_Trigger_HideNonCinematicActors);
}

void UDestroyActorCinematicEventAction::Execute_Implementation(const UCinematicContextHandle* ContextHandle) const
{
	if (IsValid(GetOwner()))
	{
		GetOwner()->Destroy();
	}
}
