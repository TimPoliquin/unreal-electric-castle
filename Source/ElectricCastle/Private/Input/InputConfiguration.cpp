// Copyright Alien Shores


#include "Input/InputConfiguration.h"

const UInputAction* UInputConfiguration::FindAbilityInputActionForTag(
	const FGameplayTag& InputTag,
	const bool bLogNotFound
) const
{
	for (const FInputConfigurationRow& CurrentAction : AbilityInputActions)
	{
		if (CurrentAction.InputTag.MatchesTagExact(InputTag))
		{
			return CurrentAction.InputAction;
		}
	}
	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to find InputAction for tag %s"), *InputTag.ToString());
	}
	return nullptr;
}
