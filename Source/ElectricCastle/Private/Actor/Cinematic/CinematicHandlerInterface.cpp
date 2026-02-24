// Copyright Alien Shores


#include "Actor/Cinematic/CinematicHandlerInterface.h"


bool ICinematicHandlerInterface::IsCinematicHandler(const UObject* Object)
{
	return IsValid(Object) && Object->Implements<UCinematicHandlerInterface>();
}

// Add default functionality here for any ICinematicHandlerInterface functions that are not pure virtual.
UCinematicHandlerComponent* ICinematicHandlerInterface::GetCinematicHandlerComponent(UObject* Object)
{
	if (IsValid(Object) && Object->Implements<UCinematicHandlerInterface>())
	{
		return Execute_GetCinematicHandlerComponent(Object);
	}
	return nullptr;
}

bool ICinematicHandlerInterface::Show(UObject* Object, const UCinematicContextHandle* CinematicContextHandle)
{
	if (IsCinematicHandler(Object))
	{
		return Execute_HandleCinematicEvent_Show(Object, CinematicContextHandle);
	}
	return false;
}

bool ICinematicHandlerInterface::Hide(UObject* Object, const UCinematicContextHandle* CinematicContextHandle)
{
	if (IsCinematicHandler(Object))
	{
		return Execute_HandleCinematicEvent_Hide(Object, CinematicContextHandle);
	}
	return false;
}

bool ICinematicHandlerInterface::StopTick(UObject* Object, const UCinematicContextHandle* CinematicContextHandle)
{
	if (IsCinematicHandler(Object))
	{
		return Execute_HandleCinematicEvent_StopTick(Object, CinematicContextHandle);
	}
	return false;
}

bool ICinematicHandlerInterface::StartTick(UObject* Object, const UCinematicContextHandle* CinematicContextHandle)
{
	if (IsCinematicHandler(Object))
	{
		return Execute_HandleCinematicEvent_StartTick(Object, CinematicContextHandle);
	}
	return false;
}

bool ICinematicHandlerInterface::MoveTo(UObject* Object, const UCinematicContextHandle* CinematicContextHandle, const FVector& MoveToLocation)
{
	if (IsCinematicHandler(Object))
	{
		return Execute_HandleCinematicEvent_MoveTo(Object, CinematicContextHandle, MoveToLocation);
	}
	return false;
}

bool ICinematicHandlerInterface::Activate(UObject* Object, const UCinematicContextHandle* CinematicContextHandle)
{
	if (IsCinematicHandler(Object))
	{
		return Execute_HandleCinematicEvent_Activate(Object, CinematicContextHandle);
	}
	return false;
}

bool ICinematicHandlerInterface::Deactivate(UObject* Object, const UCinematicContextHandle* CinematicContextHandle)
{
	if (IsCinematicHandler(Object))
	{
		return Execute_HandleCinematicEvent_Deactivate(Object, CinematicContextHandle);
	}
	return false;
}

bool ICinematicHandlerInterface::OnBegin(UObject* Object, const UCinematicContextHandle* CinematicContextHandle)
{
	if (IsCinematicHandler(Object))
	{
		return Execute_HandleCinematicEvent_OnBegin(Object, CinematicContextHandle);
	}
	return false;
}

bool ICinematicHandlerInterface::OnEnd(UObject* Object, const UCinematicContextHandle* CinematicContextHandle)
{
	if (IsCinematicHandler(Object))
	{
		return Execute_HandleCinematicEvent_OnEnd(Object, CinematicContextHandle);
	}
	return false;
}
