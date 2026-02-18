// Copyright Alien Shores


#include "Player/Input/RadialInputListenerInterface.h"


// Add default functionality here for any IRadialInputListenerInterface functions that are not pure virtual.
bool IRadialInputListenerInterface::IsRadialInputListener(const UObject* Object)
{
	return IsValid(Object) && Object->Implements<URadialInputListenerInterface>();
}

void IRadialInputListenerInterface::OnRadialInputAngleChange(UObject* Object, const float Value)
{
	if (IsRadialInputListener(Object))
	{
		Execute_OnRadialInputAngleChange(Object, Value);
	}
}
