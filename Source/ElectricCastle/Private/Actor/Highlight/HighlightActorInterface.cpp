// Copyright Alien Shores

#include "Actor/Highlight/HighlightActorInterface.h"

#include "Actor/Highlight/HighlightComponent.h"


bool IHighlightActorInterface::IsHighlightActor(const UObject* Object)
{
	return IsValid(Object) && Object->Implements<UHighlightActorInterface>();
}

// Add default functionality here for any IHighlightActorInterface functions that are not pure virtual.
UHighlightComponent* IHighlightActorInterface::GetHighlightComponent(const UObject* Actor)
{
	if (IsHighlightActor(Actor))
	{
		return Execute_GetHighlightComponent(Actor);
	}
	return nullptr;
}

void IHighlightActorInterface::Highlight(const UObject* Actor)
{
	if (UHighlightComponent* HighlightComponent = GetHighlightComponent(Actor))
	{
		HighlightComponent->Highlight();
	}
}

void IHighlightActorInterface::Unhighlight(const UObject* Actor)
{
	if (UHighlightComponent* HighlightComponent = GetHighlightComponent(Actor))
	{
		HighlightComponent->Unhighlight();
	}
}
