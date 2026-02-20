// Copyright Alien Shores


#include "Player/SelectionWheel//SelectionWheelManagerActorInterface.h"


// Add default functionality here for any IRadialInputDispatcherInterface functions that are not pure virtual.
USelectionWheelManagerComponent* ISelectionWheelManagerActorInterface::GetSelectionWheelManagerComponent(const UObject* Object)
{
	if (IsValid(Object) && Object->Implements<USelectionWheelManagerActorInterface>())
	{
		return Execute_GetSelectionWheelManagerComponent(Object);
	}
	return nullptr;
}
