// Copyright Alien Shores


#include "Actor/Attack/Component/AttackWindowManager.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actor/Attack/Window/AttackWindow.h"

#include "ElectricCastle/ElectricCastleLogChannels.h"


// Sets default values for this component's properties
UAttackWindowManager::UAttackWindowManager()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAttackWindowManager::BeginPlay()
{
	Super::BeginPlay();
	SetComponentTickEnabled(false);
}

void UAttackWindowManager::SetAbilitySystemComponent(UAbilitySystemComponent* InAbilitySystemComponent)
{
	AbilitySystemComponent = InAbilitySystemComponent;
}

void UAttackWindowManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!IsValid(AbilitySystemComponent))
	{
		return;
	}
	TArray<FHitResult> Hits;
	for (UAttackWindow* AttackWindow : ActiveAttackWindows)
	{
		Hits.Empty();
		if (!AttackWindow->SweepForHits(DeltaTime, Hits))
		{
			continue;
		}
		for (const FHitResult& HitResult : Hits)
		{
			HandleAttackWindowHit(AttackWindow, HitResult);
		}
	}
}

void UAttackWindowManager::StartAttackWindow(UAttackWindow* AttackWindow)
{
	if (!AbilitySystemComponent)
	{
		if (UAbilitySystemComponent* OwnerAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
		{
			SetAbilitySystemComponent(OwnerAbilitySystemComponent);
		}
		else
		{
			UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] No ability system component on owner - cancelling attack window!"), *GetOwner()->GetName(), *GetName());
			return;
		}
	}
	ActiveAttackWindows.AddUnique(AttackWindow);
	SetComponentTickEnabled(true);
}

void UAttackWindowManager::EndAttackWindow(UAttackWindow* AttackWindow)
{
	ActiveAttackWindows.Remove(AttackWindow);
	SetComponentTickEnabled(!ActiveAttackWindows.IsEmpty());
}

void UAttackWindowManager::HandleAttackWindowHit(const UAttackWindow* AttackWindow, const FHitResult& HitResult) const
{
	FGameplayEventData Payload;
	Payload.ContextHandle = AbilitySystemComponent->MakeEffectContext();
	Payload.ContextHandle.AddHitResult(HitResult);
	Payload.ContextHandle.AddSourceObject(GetOwner());
	Payload.ContextHandle.AddInstigator(GetOwner(), GetOwner());
	Payload.EventTag = AttackWindow->GetEventTag();
	Payload.Instigator = GetOwner();
	Payload.Target = HitResult.GetActor();
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(), AttackWindow->GetEventTag(), Payload);
}
