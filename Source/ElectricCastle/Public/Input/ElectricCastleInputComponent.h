// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "InputConfiguration.h"
#include "EnhancedInputComponent.h"
#include "ElectricCastleInputComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API UElectricCastleInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	UElectricCastleInputComponent();

	template <
		class UserClass,
		typename PressedFuncType,
		typename ReleasedFuncType,
		typename HeldFuncType
	>
	void BindAbilityActions(
		const UInputConfiguration* InputConfig,
		UserClass* Object,
		PressedFuncType PressedFunc,
		ReleasedFuncType ReleasedFunc,
		HeldFuncType HeldFunc
	);
};

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
void UElectricCastleInputComponent::BindAbilityActions(
	const UInputConfiguration* InputConfig,
	UserClass* Object,
	PressedFuncType PressedFunc,
	ReleasedFuncType ReleasedFunc,
	HeldFuncType HeldFunc
)
{
	check(InputConfig);
	for (const auto& [InputAction, InputTag] : InputConfig->AbilityInputActions)
	{
		if (InputAction && InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindAction(InputAction, ETriggerEvent::Started, Object, PressedFunc, InputTag);
			}
			if (ReleasedFunc)
			{
				BindAction(InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, InputTag);
			}
			if (HeldFunc)
			{
				BindAction(InputAction, ETriggerEvent::Triggered, Object, HeldFunc, InputTag);
			}
		}
	}
}
