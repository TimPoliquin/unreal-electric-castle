// Copyright Alien Shores


#include "Player/SelectionWheel//SelectionWheelSubscriberInterface.h"


// Add default functionality here for any IRadialInputListenerInterface functions that are not pure virtual.
bool ISelectionWheelSubscriberInterface::IsSelectionWheelSubscriber(const UObject* Object)
{
	return IsValid(Object) && Object->Implements<USelectionWheelSubscriberInterface>();
}

void ISelectionWheelSubscriberInterface::OnSelectionWheelAngleChange(UObject* Object, const float Value)
{
	if (IsSelectionWheelSubscriber(Object))
	{
		Execute_OnSelectionWheelAngleChange(Object, Value);
	}
}

void ISelectionWheelSubscriberInterface::OnSelectionWheelConfirm(UObject* Object)
{
	if (IsSelectionWheelSubscriber(Object))
	{
		Execute_OnSelectionWheelConfirm(Object);
	}
}
