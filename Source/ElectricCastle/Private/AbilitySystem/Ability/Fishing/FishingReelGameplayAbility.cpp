// Copyright Alien Shores


#include "AbilitySystem/Ability/Fishing/FishingReelGameplayAbility.h"

#include "Fishing/FishingComponent.h"
#include "Interaction/FishingActorInterface.h"

void UFishingReelGameplayAbility::ReelIn(AActor* Player)
{
	if (UFishingComponent* FishingComponent = IFishingActorInterface::GetFishingComponent(Player))
	{
		FishingComponent->OnFishingStateChangedDelegate.AddDynamic(
			this,
			&UFishingReelGameplayAbility::OnFishingStateChanged
		);
		FishingComponent->Reel();
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}

void UFishingReelGameplayAbility::OnFishingStateChanged(EFishingState FishingState)
{
	if (FishingState != EFishingState::Reeling)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}
