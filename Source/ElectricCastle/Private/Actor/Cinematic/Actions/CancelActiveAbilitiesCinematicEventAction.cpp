// Copyright Alien Shores


#include "Actor/Cinematic/Actions/CancelActiveAbilitiesCinematicEventAction.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Tags/ElectricCastleGameplayTags.h"

UCancelActiveAbilitiesCinematicEventAction::UCancelActiveAbilitiesCinematicEventAction()
{
	Triggers.AddTag(FElectricCastleGameplayTags::Get().Cinematic_Reaction_Trigger_HideNonCinematicActors);
}

void UCancelActiveAbilitiesCinematicEventAction::Execute_Implementation(const UCinematicContextHandle* ContextHandle) const
{
	if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
	{
		AbilitySystemComponent->CancelAllAbilities();
	}
}
