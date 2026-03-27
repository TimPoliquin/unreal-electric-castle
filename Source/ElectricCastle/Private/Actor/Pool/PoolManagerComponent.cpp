// Copyright Alien Shores


#include "Actor/Pool/PoolManagerComponent.h"

#include "Actor/Pool/SpawnPoolInterface.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"

UPoolManagerComponent::UPoolManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPoolManagerComponent::SetSpawnPool(UObject* InSpawnPoolComponent)
{
	if (IsValid(InSpawnPoolComponent) && InSpawnPoolComponent->Implements<USpawnPoolInterface>())
	{
		SpawnPoolComponent = InSpawnPoolComponent;
	}
	else
	{
		UE_LOG(
			LogElectricCastle,
			Error,
			TEXT("[%s:%s] Attempted to set invalid spawn pool: %s"),
			*GetOwner()->GetName(),
			*GetName(),
			InSpawnPoolComponent ? *InSpawnPoolComponent->GetName() : TEXT("nullptr")
		);
	}
}

void UPoolManagerComponent::HandleBeginRetrieveFromPool()
{
	OnBeginRetrieve.Broadcast(FSpawnPoolEventPayload(GetOwner(), this));
}

void UPoolManagerComponent::HandleFinishRetrieveFromPool()
{
	if (bMakeVisibleOnRetrieve)
	{
		GetOwner()->SetActorHiddenInGame(false);
	}
	if (bEnableActorCollisionOnRetrieve)
	{
		GetOwner()->SetActorEnableCollision(true);
	}
	if (bEnableTickOnRetrieve)
	{
		GetOwner()->SetActorTickEnabled(true);
	}
	if (bAutoReturn && AutoReturnTime > 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(
			ReturnTimer,
			[this]()
			{
				ReturnToPool();
			},
			AutoReturnTime,
			false
		);
	}
	OnFinishRetrieve.Broadcast(FSpawnPoolEventPayload(GetOwner(), this));
}

void UPoolManagerComponent::HandleReturnedToPool()
{
	if (bHideOnReturn)
	{
		GetOwner()->SetActorHiddenInGame(true);
	}
	if (bDisableActorCollisionOnReturn)
	{
		GetOwner()->SetActorEnableCollision(false);
	}
	if (bDisableTickOnReturn)
	{
		GetOwner()->SetActorTickEnabled(false);
	}
	GetWorld()->GetTimerManager().ClearTimer(ReturnTimer);
	ReturnTimer.Invalidate();
	OnReturnToPool.Broadcast(FSpawnPoolEventPayload(GetOwner(), this));
}

void UPoolManagerComponent::ReturnToPool() const
{
	if (ISpawnPoolInterface::IsSpawnPool(SpawnPoolComponent.Get()))
	{
		ISpawnPoolInterface::ReturnToPool(SpawnPoolComponent.Get(), GetOwner());
	}
}
