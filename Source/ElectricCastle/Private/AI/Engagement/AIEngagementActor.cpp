// Copyright Alien Shores


#include "AI/Engagement/AIEngagementActor.h"

UAIEngagementController* IAIEngagementActor::GetAIEngagementController(const UObject* Actor)
{
	if (IsValid(Actor) && Actor->Implements<UAIEngagementActor>())
	{
		return Execute_GetAIEngagementController(Actor);
	}
	return nullptr;
}
