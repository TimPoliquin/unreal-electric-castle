// Copyright Alien Shores


#include "UI/Widget/Layout/RadialLayoutCursor.h"

#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Player/ElectricCastlePlayerController.h"
#include "Player/Input/RadialUIInputComponent.h"

void URadialLayoutCursor::NativeConstruct()
{
	Super::NativeConstruct();
	if (URadialUIInputComponent* RadialUIInputComponent = IRadialInputDispatcherInterface::GetRadialUIInputComponent(GetOwningPlayer()))
	{
		RadialUIInputComponent->AddListener(this);
	}
	else
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[%s] PlayerController is not valid"), *GetName());
	}
}

void URadialLayoutCursor::NativeDestruct()
{
	Super::NativeDestruct();
	if (URadialUIInputComponent* RadialUIInputComponent = IRadialInputDispatcherInterface::GetRadialUIInputComponent(GetOwningPlayer()))
	{
		RadialUIInputComponent->RemoveListener(this);
	}
}

void URadialLayoutCursor::OnRadialInputAngleChange_Implementation(float Value)
{
	SetRenderTransformAngle(Value);
}
