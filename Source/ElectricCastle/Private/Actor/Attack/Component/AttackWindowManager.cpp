// Copyright Alien Shores


#include "Actor/Attack/Component/AttackWindowManager.h"

#include "Actor/Attack/Window/AttackWindow.h"

#include "ElectricCastle/ElectricCastleLogChannels.h"


// Sets default values for this component's properties
UAttackWindowManager::UAttackWindowManager()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAttackWindowManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	TArray<FHitResult> Hits;
	for (UAttackWindow* AttackWindow : ActiveAttackWindows)
	{
		AttackWindow->SweepForHits(DeltaTime, Hits);
	}
	if (Hits.Num() > 0)
	{
		if (bDebug)
		{
			UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] Found hits: %d"), *GetOwner()->GetName(), *GetName(), Hits.Num());
		}
		OnAttackHit.Broadcast(FAttackWindowHitPayload(GetOwner(), this, Hits));
	}
}

void UAttackWindowManager::StartAttackWindow(UAttackWindow* AttackWindow)
{
	ActiveAttackWindows.AddUnique(AttackWindow);
	SetComponentTickEnabled(true);
}

void UAttackWindowManager::EndAttackWindow(UAttackWindow* AttackWindow)
{
	ActiveAttackWindows.Remove(AttackWindow);
	SetComponentTickEnabled(!ActiveAttackWindows.IsEmpty());
}
