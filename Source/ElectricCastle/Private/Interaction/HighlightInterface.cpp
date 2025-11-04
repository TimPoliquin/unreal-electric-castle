// Copyright Alien Shores


#include "Interaction/HighlightInterface.h"


// Add default functionality here for any IIHighlightInterface functions that are not pure virtual.
void IHighlightInterface::HighlightActor(UObject* Actor)
{
	if (IsValid(Actor) && Actor->Implements<UHighlightInterface>())
	{
		Execute_HighlightActor(Actor);
	}
}

void IHighlightInterface::UnHighlightActor(UObject* Actor)
{
	if (IsValid(Actor) && Actor->Implements<UHighlightInterface>())
	{
		Execute_UnHighlightActor(Actor);
	}
}

bool IHighlightInterface::IsHighlightActor(const UObject* Actor)
{
	return IsValid(Actor) && Actor->Implements<UHighlightInterface>();
}

bool IHighlightInterface::SetMoveToLocation(UObject* Actor, FVector& OutDestination)
{
	if (IsValid(Actor) && Actor->Implements<UHighlightInterface>())
	{
		return Execute_SetMoveToLocation(Actor, OutDestination);
	}
	return false;
}
