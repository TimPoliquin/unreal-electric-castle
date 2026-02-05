// Copyright Alien Shores


#include "Actor/BeamTargetInterface.h"


// Add default functionality here for any IBeamTargetInterface functions that are not pure virtual.
bool IBeamTargetInterface::IsBeamTarget(const UObject* Object)
{
	return IsValid(Object) && Object->Implements<UBeamTargetInterface>();
}
