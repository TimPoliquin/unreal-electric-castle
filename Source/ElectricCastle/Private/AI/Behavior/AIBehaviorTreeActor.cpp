// Copyright Alien Shores


#include "AI/Behavior/AIBehaviorTreeActor.h"


// Add default functionality here for any IAIBehaviorTreeActor functions that are not pure virtual.
UBehaviorTree* IAIBehaviorTreeActor::GetBehaviorTree(const UObject* Object)
{
	return IsValid(Object) && Object->Implements<UAIBehaviorTreeActor>() ? Execute_GetBehaviorTree(Object) : nullptr;
}

bool IAIBehaviorTreeActor::ShouldAutoRunBehaviorTree(const UObject* Object)
{
	return IsValid(Object) && Object->Implements<UAIBehaviorTreeActor>() ? Execute_ShouldAutoRunBehaviorTree(Object) : false;
}
