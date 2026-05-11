// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "InputConfiguration.h"
#include "EnhancedInputComponent.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
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

	void UnbindAbilityActions(UInputConfiguration* InputConfiguration);

private:
	TMap<FGameplayTag, TArray<FInputBindingHandle>> InputBindings;
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
	if (!IsValid(InputConfig))
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[%s:%s] Attempted to apply invalid input config"), *GetOwner()->GetName(), *GetName())
		return;
	}
	if (!IsValid(Object))
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[%s:%s] Attempted to apply input config %s to invalid object"), *GetOwner()->GetName(), *GetName(), *InputConfig->GetName())
		return;
	}
	UE_LOG(LogElectricCastle, Log, TEXT("[%s:%s] Applying input config %s to object: %s"), *GetOwner()->GetName(), *GetName(), *InputConfig->GetName(), *Object->GetName())
	for (const auto& [InputAction, InputTag, TriggerEvents] : InputConfig->AbilityInputActions)
	{
		if (InputAction && InputTag.IsValid())
		{
			TArray<FInputBindingHandle> Handles;
			if (PressedFunc && TriggerEvents.Contains(ETriggerEvent::Started))
			{
				Handles.Add(BindAction(InputAction, ETriggerEvent::Started, Object, PressedFunc, InputTag));
			}
			if (ReleasedFunc && TriggerEvents.Contains(ETriggerEvent::Completed))
			{
				Handles.Add(BindAction(InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, InputTag));
			}
			if (HeldFunc && TriggerEvents.Contains(ETriggerEvent::Triggered))
			{
				Handles.Add(BindAction(InputAction, ETriggerEvent::Triggered, Object, HeldFunc, InputTag));
			}
			InputBindings.Add(InputTag, Handles);
		}
	}
}
