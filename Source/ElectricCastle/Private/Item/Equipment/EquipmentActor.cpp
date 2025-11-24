// Copyright Alien Shores


#include "Item/Equipment/EquipmentActor.h"


// Add default functionality here for any IEquipmentActor functions that are not pure virtual.
FGameplayTag IEquipmentActor::GetItemType(const UObject* Object)
{
	if (IsValid(Object) && Object->Implements<UEquipmentActor>())
	{
		return Execute_GetItemType(Object);
	}
	return FGameplayTag::EmptyTag;
}
