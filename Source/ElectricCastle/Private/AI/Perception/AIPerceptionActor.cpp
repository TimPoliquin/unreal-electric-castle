// Copyright Alien Shores


#include "AI/Perception/AIPerceptionActor.h"


bool IAIPerceptionActor::IsAIPerceptionActor(const UObject* Actor)
{
	if (IsValid(Actor) && Actor->Implements<UAIPerceptionActor>())
	{
		return true;
	}
	return false;
}

UAIPerceptionComponent* IAIPerceptionActor::GetAIPerceptionComponent(const UObject* Actor)
{
	if (IsAIPerceptionActor(Actor))
	{
		return Execute_GetAIPerceptionComponent(Actor);
	}
	return nullptr;
}
