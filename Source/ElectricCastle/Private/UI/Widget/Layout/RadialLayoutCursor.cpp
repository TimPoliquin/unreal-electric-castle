// Copyright Alien Shores


#include "UI/Widget/Layout/RadialLayoutCursor.h"

#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Player/SelectionWheel/SelectionWheelManagerActorInterface.h"
#include "Player/SelectionWheel/SelectionWheelManagerComponent.h"

void URadialLayoutCursor::OnSelectionWheelAngleChange_Implementation(float Value)
{
	SetRenderTransformAngle(Value);
}

void URadialLayoutCursor::OnSelectionWheelConfirm_Implementation()
{
	// TODO
}
