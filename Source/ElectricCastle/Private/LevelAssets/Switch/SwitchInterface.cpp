// Copyright Alien Shores


#include "LevelAssets/Switch/SwitchInterface.h"


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
