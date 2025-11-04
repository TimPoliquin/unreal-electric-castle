// Copyright Alien Shores


#include "Player/Form/FormChangeActorInterface.h"


UPlayerFormChangeComponent* IFormChangeActorInterface::GetFormChangeComponent(const UObject* Object)
{
	if (Object && Object->Implements<UFormChangeActorInterface>())
	{
		return Execute_GetFormChangeComponent(Object);
	}
	return nullptr;
}
