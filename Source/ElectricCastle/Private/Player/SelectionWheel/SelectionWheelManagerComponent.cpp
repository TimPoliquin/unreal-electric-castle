// Copyright Alien Shores


#include "Player/SelectionWheel//SelectionWheelManagerComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "IContentBrowserSingleton.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Input/ElectricCastleInputComponent.h"
#include "Input/Utils/PlayerInputFunctionLibrary.h"
#include "Player/SelectionWheel/SelectionWheelSubscriberInterface.h"


USelectionWheelManagerComponent::USelectionWheelManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

bool USelectionWheelManagerComponent::IsSelectionWheelActive() const
{
	return bHasAddedContext;
}

void USelectionWheelManagerComponent::SetupInputComponent(UElectricCastleInputComponent* InputComponent)
{
	InputComponent->BindAction(SelectInputAction, ETriggerEvent::Triggered, this, &USelectionWheelManagerComponent::UpdateAngle);
	InputComponent->BindAction(ConfirmInputAction, ETriggerEvent::Started, this, &USelectionWheelManagerComponent::ConfirmSelection);
}

void USelectionWheelManagerComponent::AddListener(UObject* Listener)
{
	if (ISelectionWheelSubscriberInterface::IsSelectionWheelSubscriber(Listener))
	{
		Listeners.AddUnique(Listener);
	}
	else
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] Object is not a valid radial input listener: %s"), *GetOwner()->GetName(), *GetName(), Listener ? *Listener->GetName() : *FString("NULLPTR"));
	}
	if (!bHasAddedContext && Listeners.Num() > 0)
	{
		UPlayerInputFunctionLibrary::AddInputMappingContext(GetOwner(), InputMappingContext);
		bHasAddedContext = true;
		OnSelectionWheelStateChanged.Broadcast(FSelectionWheelStateChangedPayload(true));
	}
}

void USelectionWheelManagerComponent::RemoveListener(const UObject* ToRemove)
{
	Listeners.RemoveAll([ToRemove](const TWeakObjectPtr<UObject> Listener) { return !Listener.IsValid() || Listener.IsStale() || Listener.Get() == ToRemove; });
	if (Listeners.Num() == 0 && bHasAddedContext)
	{
		UPlayerInputFunctionLibrary::RemoveInputMappingContext(GetOwner(), InputMappingContext);
		bHasAddedContext = false;
		OnSelectionWheelStateChanged.Broadcast(FSelectionWheelStateChangedPayload(false));
	}
}

void USelectionWheelManagerComponent::UpdateAngle(const FInputActionValue& InputActionValue)
{
	if (Listeners.Num() > 0)
	{
		const FVector2D InputDirection = InputActionValue.Get<FVector2D>();
		bool bBroadcast;
		switch (InputType)
		{
		case ECommonInputType::Gamepad:
			bBroadcast = CalculateFormWheelAngle_Gamepad(InputDirection, CurrentAngle);
			break;
		case ECommonInputType::MouseAndKeyboard:
			bBroadcast = CalculateFormWheelAngle_Mouse(InputDirection, CurrentAngle);
			break;
		default:
			bBroadcast = false;
		}
		if (bBroadcast)
		{
			NotifyListeners();
		}
	}
}

void USelectionWheelManagerComponent::ConfirmSelection(const FInputActionValue& InputActionValue)
{
	if (Listeners.Num() == 0)
	{
		return;
	}
	TArray<UObject*> CurrentListeners;
	// copy the list first - it's possible that confirmation could result in de-registration, which
	// would mutate the array mid-iteration.
	for (TWeakObjectPtr<UObject> Listener : Listeners)
	{
		if (ISelectionWheelSubscriberInterface::IsSelectionWheelSubscriber(Listener.Get()))
		{
			CurrentListeners.Add(Listener.Get());
		}
	}
	for (UObject* Listener : CurrentListeners)
	{
		ISelectionWheelSubscriberInterface::OnSelectionWheelConfirm(Listener);
	}
}

bool USelectionWheelManagerComponent::CalculateFormWheelAngle_Gamepad(const FVector2D& InputDirection, float& OutFormWheelAngle) const
{
	const float NewValue = FMath::RadiansToDegrees(FMath::Atan2(InputDirection.X, -InputDirection.Y));
	if (FMath::IsNearlyEqual(NewValue, CurrentAngle))
	{
		return false;
	}
	OutFormWheelAngle = NewValue;
	return true;
}

bool USelectionWheelManagerComponent::CalculateFormWheelAngle_Mouse(const FVector2D& InputDirection, float& OutFormWheelAngle) const
{
	const float AbsX = FMath::Abs(InputDirection.X);
	const float AbsY = FMath::Abs(InputDirection.Y);
	const float Value = AbsX > AbsY ? InputDirection.X : InputDirection.Y;
	if (FMath::Abs(Value) > KINDA_SMALL_NUMBER)
	{
		OutFormWheelAngle += Value * MouseSensitivity;
		// Normalize to [0,360)
		OutFormWheelAngle = FMath::Fmod(OutFormWheelAngle + 360.0f, 360.0f);
		return true;
	}
	return false;
}

void USelectionWheelManagerComponent::RemoveStaleListeners()
{
	Listeners.RemoveAll([](const TWeakObjectPtr<UObject> Listener) { return !Listener.IsValid() || Listener.IsStale(); });
	if (Listeners.Num() == 0 && bHasAddedContext)
	{
		UPlayerInputFunctionLibrary::RemoveInputMappingContext(GetOwner(), InputMappingContext);
		bHasAddedContext = false;
		FSelectionWheelStateChangedPayload(false);
	}
}

void USelectionWheelManagerComponent::NotifyListeners()
{
	RemoveStaleListeners();
	for (TWeakObjectPtr<UObject> Listener : Listeners)
	{
		ISelectionWheelSubscriberInterface::OnSelectionWheelAngleChange(Listener.Get(), CurrentAngle);
	}
}
