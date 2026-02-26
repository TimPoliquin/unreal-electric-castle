// Copyright Alien Shores


#include "Actor/Cinematic/Actions/DisableTriggerCinematicEventAction.h"

#include "Actor/TriggerInterface.h"
#include "Cinematic/Context/CinematicContextHandle.h"
#include "Tags/ElectricCastleGameplayTags.h"

UDisableTriggerCinematicEventAction::UDisableTriggerCinematicEventAction()
{
	Triggers.AddTag(FElectricCastleGameplayTags::Get().Cinematic_Reaction_Trigger_HideNonCinematicActors);
}

void UDisableTriggerCinematicEventAction::Execute_Implementation(const UCinematicContextHandle* ContextHandle) const
{
	if (ITriggerInterface::IsTriggered(GetOwner()))
	{
		ContextHandle->AddRestoreLambda(GetOwner(), ITriggerInterface::IsTriggered(GetOwner()), [this](AActor* Actor, const bool& bInValue)
		{
			ITriggerInterface::Trigger(Actor, bInValue);
		});
		ITriggerInterface::Trigger(GetOwner(), false);
	}
}
