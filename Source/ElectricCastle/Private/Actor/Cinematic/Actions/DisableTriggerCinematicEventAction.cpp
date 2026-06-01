// Copyright Alien Shores


#include "Actor/Cinematic/Actions/DisableTriggerCinematicEventAction.h"

#include "Actor/Trigger/TriggerableInterface.h"
#include "Cinematic/Context/CinematicContextHandle.h"
#include "Tags/ElectricCastleGameplayTags.h"

UDisableTriggerCinematicEventAction::UDisableTriggerCinematicEventAction()
{
	Triggers.AddTag(FElectricCastleGameplayTags::Get().Cinematic_Reaction_Trigger_HideNonCinematicActors);
}

void UDisableTriggerCinematicEventAction::Execute_Implementation(const UCinematicContextHandle* ContextHandle) const
{
	if (ITriggerableInterface::IsTriggered(GetOwner()))
	{
		ContextHandle->AddRestoreLambda(
			GetOwner(),
			ITriggerableInterface::IsTriggered(GetOwner()),
			[this](AActor* Actor, const bool& bInValue)
			{
				ITriggerableInterface::Trigger(Actor, bInValue);
			}
		);
		ITriggerableInterface::Trigger(GetOwner(), false);
	}
}
