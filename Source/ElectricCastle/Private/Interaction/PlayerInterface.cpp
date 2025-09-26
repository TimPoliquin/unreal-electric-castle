// Copyright Alien Shores


#include "Interaction/PlayerInterface.h"


// Add default functionality here for any IPlayerInterface functions that are not pure virtual.
void IPlayerInterface::AddToXP(AActor* Actor, int32 InXP)
{
	if (Actor->Implements<UPlayerInterface>())
	{
		Execute_AddToXP(Actor, InXP);
	}
}

bool IPlayerInterface::ImplementsPlayerInterface(const UObject* Object)
{
	return Object && Object->Implements<UPlayerInterface>();
}

int32 IPlayerInterface::GetAttributePoints(const UObject* Object)
{
	if (Object && Object->Implements<UPlayerInterface>())
	{
		return Execute_GetAttributePoints(Object);
	}
	return 0;
}

int32 IPlayerInterface::GetSpellPoints(const UObject* Object)
{
	if (Object && Object->Implements<UPlayerInterface>())
	{
		return Execute_GetSpellPoints(Object);
	}
	return 0;
}

void IPlayerInterface::SpendAttributePoints(UObject* Object, int32 SpentPoints)
{
	if (Object && Object->Implements<UPlayerInterface>())
	{
		Execute_SpendAttributePoints(Object, SpentPoints);
	}
}

void IPlayerInterface::SpendSpellPoints(UObject* Object, const int32 SpentPoints)
{
	if (Object && Object->Implements<UPlayerInterface>())
	{
		Execute_SpendSpellPoints(Object, SpentPoints);
	}
}

void IPlayerInterface::SaveProgress(UObject* Object, const FName& CheckpointTag)
{
	if (IsValid(Object) && Object->Implements<UPlayerInterface>())
	{
		Execute_SaveProgress(Object, CheckpointTag);
	}
}
