#include "Actor/Highlight/HighlightTypes.h"

#include "Actor/Highlight/HighlightActorInterface.h"

void FHighlightContext::Track(AActor* Actor)
{
	LastActor = CurrentActor;
	if (IHighlightActorInterface::IsHighlightActor(Actor))
	{
		CurrentActor = Actor;
	}
	else
	{
		CurrentActor = nullptr;
	}
	if (IsDifferentPtr())
	{
		UnHighlightLast();
		HighlightCurrent();
	}
}

void FHighlightContext::Clear()
{
	UnHighlightCurrent();
	UnHighlightLast();
	CurrentActor = nullptr;
	LastActor = nullptr;
}

bool FHighlightContext::HasCurrentTarget() const
{
	return CurrentActor != nullptr;
}

bool FHighlightContext::IsDifferentPtr() const
{
	return LastActor != CurrentActor;
}

void FHighlightContext::HighlightCurrent() const
{
	if (CurrentActor != nullptr)
	{
		IHighlightActorInterface::Highlight(CurrentActor);
	}
}

void FHighlightContext::UnHighlightCurrent() const
{
	if (CurrentActor != nullptr)
	{
		IHighlightActorInterface::Unhighlight(CurrentActor);
	}
}

void FHighlightContext::UnHighlightLast() const
{
	if (LastActor != nullptr)
	{
		IHighlightActorInterface::Unhighlight(LastActor);
	}
}
