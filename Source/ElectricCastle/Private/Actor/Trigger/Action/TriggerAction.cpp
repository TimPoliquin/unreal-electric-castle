// Copyright Alien Shores


#include "Actor/Trigger/Action/TriggerAction.h"

#include "Actor/Trigger/TriggerableInterface.h"
#include "GameFramework/Actor.h"

bool UTriggerAction::ShouldExecuteTrigger_Implementation(const AActor* InTriggerActor) const
{
	return IsValid(InTriggerActor) && InTriggerActor->Implements<UTriggerableInterface>();
}

void UTriggerAction::ExecuteAction_Implementation(AActor* TargetActor)
{
	ITriggerableInterface::Trigger(TargetActor, true);
}

void UTriggerAction::SetOwnerActor(AActor* InOwner)
{
	OwnerActor = InOwner;
}

void UTriggerAction::Execute()
{
	if (ShouldExecuteTrigger(TriggerActor))
	{
		ExecuteAction(TriggerActor);
	}
}
