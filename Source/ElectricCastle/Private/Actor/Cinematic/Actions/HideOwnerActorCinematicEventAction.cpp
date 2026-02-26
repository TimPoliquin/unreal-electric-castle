// Copyright Alien Shores


#include "Actor/Cinematic/Actions/HideOwnerActorCinematicEventAction.h"

#include "Tags/ElectricCastleGameplayTags.h"

UHideOwnerActorCinematicEventAction::UHideOwnerActorCinematicEventAction()
{
	Triggers.AddTag(FElectricCastleGameplayTags::Get().Cinematic_Reaction_Trigger_HideNonCinematicActors);
}

void UHideOwnerActorCinematicEventAction::GetActorsToHide_Implementation(TArray<AActor*>& OutActors) const
{
	if (IsValid(GetOwner()))
	{
		OutActors.AddUnique(GetOwner());
	}
}
