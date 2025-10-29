// Copyright Alien Shores


#include "Player/Equipment/EquipmentManagerInterface.h"


// Add default functionality here for any IEquipmentManagerInterface functions that are not pure virtual.
UPlayerEquipmentComponent* IEquipmentManagerInterface::GetEquipmentComponent(const UObject* Object)
{
	if (IsValid(Object) && Object->Implements<UEquipmentManagerInterface>())
	{
		return Execute_GetEquipmentComponent(Object);
	}
	return nullptr;
}
