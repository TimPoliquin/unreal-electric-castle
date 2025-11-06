// Copyright Alien Shores


#include "Actor/State/StateShiftInterface.h"

#include "Actor/State/StateShiftComponent.h"

UStateShiftComponent* IStateShiftInterface::GetStateShiftComponent(UObject* Object)
{
	if (IsValid(Object) && Object->Implements<UStateShiftInterface>())
	{
		return Cast<IStateShiftInterface>(Object)->GetStateShiftComponent();
	}
	return nullptr;
}
