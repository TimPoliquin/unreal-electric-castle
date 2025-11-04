// Copyright Alien Shores


#include "FishingSuccessWidgetInterface.h"

#include "Fishing/FishCatch.h"


// Add default functionality here for any IFishingSuccessWidgetInterface functions that are not pure virtual.
void IFishingSuccessWidgetInterface::SetFishingCatchInfo(UObject* FishingSuccessWidget, const UFishCatch* FishCatch)
{
	if (IsValid(FishingSuccessWidget) && FishingSuccessWidget->Implements<UFishingSuccessWidgetInterface>())
	{
		Execute_SetFishingCatchInfo(FishingSuccessWidget, FishCatch);
	}
}
