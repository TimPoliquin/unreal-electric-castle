// Copyright Alien Shores


#include "Item/Pickup/TieredItemInterface.h"


// Add default functionality here for any ITieredItemInterface functions that are not pure virtual.
void ITieredItemInterface::SetItemLevel(UObject* Object, int32 Level)
{
	if (Object->Implements<UTieredItemInterface>())
	{
		Execute_SetItemLevel(Object, Level);
	}
}
