// Copyright Alien Shores


#include "AI/Perception/AIPerceptionManagerActor.h"

#include "AI/Perception/AIPerceptionManager.h"


// Add default functionality here for any IAIPerceptionManagerActor functions that are not pure virtual.
UAIPerceptionManager* IAIPerceptionManagerActor::GetAIPerceptionManager(const UObject* WorldContext)
{
	return IsValid(WorldContext) && WorldContext->Implements<UAIPerceptionManagerActor>() ? Execute_GetAIPerceptionManager(WorldContext) : nullptr;
}
