// Copyright Alien Shores


#include "Actor/Cinematic/Actions/DisableHighlightCinematicEventAction.h"

#include "Actor/Highlight/HighlightActorInterface.h"
#include "Actor/Highlight/HighlightComponent.h"
#include "Cinematic/Context/CinematicContextHandle.h"
#include "Tags/ElectricCastleGameplayTags.h"


UDisableHighlightCinematicEventAction::UDisableHighlightCinematicEventAction()
{
	Triggers.AddTag(FElectricCastleGameplayTags::Get().Cinematic_Reaction_Trigger_HideNonCinematicActors);
}

void UDisableHighlightCinematicEventAction::Execute_Implementation(const UCinematicContextHandle* ContextHandle) const
{
	if (UHighlightComponent* HighlightComponent = IHighlightActorInterface::GetHighlightComponent(GetOwner()))
	{
		if (HighlightComponent->IsHighlightable())
		{
			ContextHandle->AddRestoreLambda(HighlightComponent, HighlightComponent->IsHighlightable(), [this](UHighlightComponent* InHighlightComponent, const bool& bInValue)
			{
				InHighlightComponent->SetHighlightable(bInValue);
			});
			HighlightComponent->SetHighlightable(false);
		}
	}
}
