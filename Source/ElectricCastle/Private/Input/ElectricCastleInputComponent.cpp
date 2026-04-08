// Copyright Alien Shores


#include "Input/ElectricCastleInputComponent.h"


UElectricCastleInputComponent::UElectricCastleInputComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UElectricCastleInputComponent::UnbindAbilityActions(UInputConfiguration* InputConfiguration)
{
	for (const auto& [InputAction, InputTag] : InputConfiguration->AbilityInputActions)
	{
		if (InputBindings.Contains(InputTag))
		{
			for (const FInputBindingHandle& Handle : InputBindings[InputTag])
			{
				RemoveBinding(Handle);
			}
			InputBindings.Remove(InputTag);
		}
	}
}
