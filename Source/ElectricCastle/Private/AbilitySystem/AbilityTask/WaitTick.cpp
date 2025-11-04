// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTask/WaitTick.h"

UWaitTick::UWaitTick(const FObjectInitializer& ObjectInitializer)
{
	bTickingTask = true;
}

UWaitTick* UWaitTick::WaitTick(UGameplayAbility* OwningAbility, FName TaskInstanceName)
{
	UWaitTick* MyObj = NewAbilityTask<UWaitTick>(OwningAbility, TaskInstanceName);
	return MyObj;
}

void UWaitTick::Activate()
{
	Super::Activate();
}

void UWaitTick::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnTick.Broadcast(DeltaTime);
	}
}
