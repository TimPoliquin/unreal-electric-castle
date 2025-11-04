// Copyright Alien Shores


#include "Item/Chest/TreasureChestActor.h"

#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Item/Component/LockComponent.h"
#include "Item/Component/LootSpawnComponent.h"

ATreasureChestActor::ATreasureChestActor()
{
	PrimaryActorTick.bCanEverTick = false;
	LockComponent = CreateDefaultSubobject<ULockComponent>(TEXT("LockComponent"));
	PreconditionWidget->SetRelativeLocation(FVector(0, 0, 150));
	PreconditionWidget->SetDrawSize(FVector2D(192, 32));
	InteractionWidget->SetRelativeLocation(FVector(1, 0, 150));
	InteractionWidget->SetDrawSize(FVector2D(192, 32));
	LootSpawnComponent = CreateDefaultSubobject<ULootSpawnComponent>(TEXT("Loot Spawn Component"));
}

bool ATreasureChestActor::IsUnlocked() const
{
	return LockComponent->IsUnlocked();
}

void ATreasureChestActor::HandleInitialState()
{
	if (State == EAuraTreasureChestState::Locked && LockComponent->IsUnlocked())
	{
		State = EAuraTreasureChestState::Unlocked;
	}
	switch (State)
	{
	case EAuraTreasureChestState::Locked:
		// do nothing
		break;
	case EAuraTreasureChestState::Unlocked:
		// do nothing
		PlayUnlockForcedEffect();
		break;
	case EAuraTreasureChestState::Open:
		PlayOpenForcedEffect();
		break;
	}
}

bool ATreasureChestActor::SetMoveToLocation_Implementation(FVector& OutDestination)
{
	OutDestination = OverlapDetectionComponent->GetComponentLocation();
	return true;
}

void ATreasureChestActor::PostLoad_Implementation()
{
	Super::PostLoad_Implementation();
	HandleInitialState();
}

void ATreasureChestActor::BeginPlay()
{
	Super::BeginPlay();
	HandleInitialState();
	if (!LockComponent->IsUnlocked())
	{
		LockComponent->OnUnlockDelegate.AddUniqueDynamic(this, &ATreasureChestActor::OnChestUnlocked);
	}
	if (LootSpawnComponent)
	{
		LootSpawnComponent->OnLootGrantCompleted.AddDynamic(this, &ATreasureChestActor::GrantRewards_Finish);
	}
}

bool ATreasureChestActor::IsPreconditionMet_Implementation(AActor* Player) const
{
	switch (State)
	{
	case EAuraTreasureChestState::Locked:
		return LockComponent->IsPreconditionMet(Player);
	default:
		return true;
	}
}

void ATreasureChestActor::Unlock(AActor* Player)
{
	if (LockComponent->IsUnlocked())
	{
		State = EAuraTreasureChestState::Unlocked;
	}
}

void ATreasureChestActor::Open(AActor* Player)
{
	State = EAuraTreasureChestState::Open;
	DisablePOI();
	PlayOpenEffect(Player);
}

void ATreasureChestActor::GrantRewards()
{
	if (LootSpawnComponent)
	{
		LootSpawnComponent->GrantLoot();
	}
}

void ATreasureChestActor::PlayOpenForcedEffect_Implementation()
{
	// No default implementation
}

void ATreasureChestActor::PlayOpenEffect_Implementation(AActor* Player)
{
	// No default implementation
}

void ATreasureChestActor::PlayUnlockForcedEffect_Implementation()
{
	// No default implementation
}

void ATreasureChestActor::PlayUnlockEffect_Implementation()
{
	// No default implementation
}

void ATreasureChestActor::HandleInteract_Implementation(AActor* Player)
{
	switch (State)
	{
	case EAuraTreasureChestState::Locked:
		if (LockComponent->IsManuallyUnlockable() && LockComponent->TryUnlock(Player))
		{
			Open(Player);
		}
		break;
	case EAuraTreasureChestState::Unlocked:
		Open(Player);
		break;
	case EAuraTreasureChestState::Open:
		// do nothing
		break;
	}
}

FTransform ATreasureChestActor::GetRewardInitialSpawnLocation_Implementation() const
{
	return GetActorTransform();
}

void ATreasureChestActor::GrantRewards_Finish_Implementation()
{
	// NO default implementation
}

void ATreasureChestActor::OnChestUnlocked(const FOnLockComponentUnlockPayload& Payload)
{
	if (State == EAuraTreasureChestState::Locked)
	{
		State = EAuraTreasureChestState::Unlocked;
		if (Payload.UnlockType != EUnlockMode::Key)
		{
			// Only play the unlock effect if the chest was unlocked by means other than a key.
			// If a key was used to unlock the chest, we'll want to jump straight to opening the chest.
			PlayUnlockEffect();
		}
	}
}
