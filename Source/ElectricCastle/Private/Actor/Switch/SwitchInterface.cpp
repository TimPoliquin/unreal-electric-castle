// Copyright Alien Shores


#include "Actor/Switch/SwitchInterface.h"


bool ISwitchInterface::IsSwitchActor(const UObject* Object)
{
	return IsValid(Object) && Object->Implements<USwitchInterface>();
}

// Add default functionality here for any ISwitchInterface functions that are not pure virtual.
bool ISwitchInterface::IsSwitchActive(const AActor* Switch)
{
	if (IsValid(Switch) && Switch->Implements<USwitchInterface>())
	{
		return Execute_IsSwitchActive(Switch);
	}
	return false;
}

void ISwitchInterface::ResetSwitch(AActor* Switch)
{
	if (IsValid(Switch) && Switch->Implements<USwitchInterface>())
	{
		return Execute_ResetSwitch(Switch);
	}
}
