// Copyright Alien Shores


#include "Player/Input/RadialUIInputComponent.h"

#include "EnhancedInputComponent.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Input/ElectricCastleInputComponent.h"
#include "Player/Input/RadialInputListenerInterface.h"


URadialUIInputComponent::URadialUIInputComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void URadialUIInputComponent::SetupInputComponent(UElectricCastleInputComponent* InputComponent)
{
	InputComponent->BindAction(InputAction, ETriggerEvent::Triggered, this, &URadialUIInputComponent::UpdateAngle);
}

void URadialUIInputComponent::AddListener(UObject* Listener)
{
	if (IRadialInputListenerInterface::IsRadialInputListener(Listener))
	{
		Listeners.AddUnique(Listener);
	}
	else
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] Object is not a valid radial input listener: %s"), *GetOwner()->GetName(), *GetName(), Listener ? *Listener->GetName() : *FString("NULLPTR"));
	}
}

void URadialUIInputComponent::RemoveListener(const UObject* ToRemove)
{
	Listeners.RemoveAll([ToRemove](const TWeakObjectPtr<UObject> Listener) { return !Listener.IsValid() || Listener.IsStale() || Listener.Get() == ToRemove; });
}

void URadialUIInputComponent::UpdateAngle(const FInputActionValue& InputActionValue)
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

bool URadialUIInputComponent::CalculateFormWheelAngle_Gamepad(const FVector2D& InputDirection, float& OutFormWheelAngle) const
{
	if (const float Magnitude = InputDirection.Size(); Magnitude > AnalogDeadZone)
	{
		OutFormWheelAngle = FMath::RadiansToDegrees(FMath::Atan2(InputDirection.X, -InputDirection.Y));
		return true;
	}
	return false;
}

bool URadialUIInputComponent::CalculateFormWheelAngle_Mouse(const FVector2D& InputDirection, float& OutFormWheelAngle) const
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

void URadialUIInputComponent::NotifyListeners()
{
	Listeners.RemoveAll([](const TWeakObjectPtr<UObject> Listener) { return !Listener.IsValid() || Listener.IsStale(); });
	for (TWeakObjectPtr<UObject> Listener : Listeners)
	{
		IRadialInputListenerInterface::OnRadialInputAngleChange(Listener.Get(), CurrentAngle);
	}
}
