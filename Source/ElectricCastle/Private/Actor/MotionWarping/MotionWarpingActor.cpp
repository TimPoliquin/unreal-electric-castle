// Copyright Alien Shores


#include "Actor/MotionWarping/MotionWarpingActor.h"

#include "MotionWarpingComponent.h"


// Add default functionality here for any IMotionWarpingActor functions that are not pure virtual.
bool IMotionWarpingActor::IsMotionWarping(const UObject* Object)
{
	if (IsValid(Object) && Object->Implements<UMotionWarpingActor>())
	{
		if (const UMotionWarpingComponent* MotionWarpingComponent = Execute_GetMotionWarpingComponent(Object))
		{
			return MotionWarpingComponent->GetModifiers().Num() > 0;
		}
	}
	return false;
}
