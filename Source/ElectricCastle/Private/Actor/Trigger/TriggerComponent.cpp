// Copyright Alien Shores


#include "Actor/Trigger/TriggerComponent.h"

#include "Actor/Trigger/Action/TriggerAction.h"


UTriggerComponent::UTriggerComponent()
{
}

void UTriggerComponent::ExecuteTriggerActions() const
{
	for (UTriggerAction* TriggerAction : Actions)
	{
		if (IsValid(TriggerAction))
		{
			TriggerAction->SetOwnerActor(GetOwner());
			TriggerAction->Execute();
		}
	}
}
