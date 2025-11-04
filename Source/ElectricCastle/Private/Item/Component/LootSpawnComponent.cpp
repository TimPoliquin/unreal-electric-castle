// Copyright Alien Shores


#include "Item/Component/LootSpawnComponent.h"

#include "AbilitySystem/ElectricCastleAbilitySystemInterface.h"
#include "Actor/Spawn/ElectricCastleSpawnBlueprintLibrary.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Game/Subsystem/ElectricCastleGameDataSubsystem.h"
#include "Item/Effect/SpawnEffectInterface.h"
#include "Item/Pickup/TreasurePickup.h"
#include "Item/Pickup/TieredItemInterface.h"
#include "Player/InventoryComponent.h"
#include "Tags/ElectricCastleGameplayTags.h"
#include "Utils/ArrayUtils.h"


// Sets default values for this component's properties
ULootSpawnComponent::ULootSpawnComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void ULootSpawnComponent::GrantLoot_Implementation()
{
	switch (GrantMode)
	{
	case EItemGrantMode::DirectToInventory:
		GrantLoot();
		break;
	case EItemGrantMode::Spawn:
		SpawnLoot();
		break;
	}
}

void ULootSpawnComponent::BeginPlay()
{
	Super::BeginPlay();
	if (!GetOwner()->Implements<UElectricCastleAbilitySystemInterface>())
	{
		for (FLootDefinition& LootDefinition : LootDefinitions)
		{
			if (!LootDefinition.bOverrideOwnerLevel)
			{
				UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] Owner does not implement ability system interface. Forcing loot definition override for item: %s"), *GetOwner()->GetName(), *GetName(),
				       *LootDefinition.ItemTag.ToString())
				LootDefinition.bOverrideOwnerLevel = true;
			}
		}
	}
}

void ULootSpawnComponent::AddLootToInventory_Implementation()
{
	if (UInventoryComponent* InventoryComponent = UInventoryComponent::Get(this))
	{
		for (const FLootDefinition& LootDefinition : LootDefinitions)
		{
			InventoryComponent->AddToInventory(LootDefinition.ItemTag);
		}
		if (GoldAmount > 0.0)
		{
			InventoryComponent->AddToInventory(FElectricCastleGameplayTags::Get().Item_Type_Treasure, GoldAmount);
		}
	}
	OnLootGrantCompleted.Broadcast();
}

FTransform ULootSpawnComponent::GetInitialLootSpawnTransform_Implementation()
{
	return GetOwner()->GetActorTransform();
}


void ULootSpawnComponent::SpawnLoot_Implementation()
{
	InstantiateLootActors(LootInstances);
	if (LootInstances.Num() > 0)
	{
		PlaySpawnItems();
	}
	else
	{
		OnLootGrantCompleted.Broadcast();
	}
}


void ULootSpawnComponent::PlaySpawnItems_Implementation()
{
	if (LootInstances.Num() <= 0)
	{
		OnLootGrantCompleted.Broadcast();
		return;
	}
	const FLootInstance& LootItem = LootInstances.Pop();
	PlaySpawnItem(LootItem);
	if (LootInstances.Num() > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(SpawnDelayTimer, [&]()
		{
			PlaySpawnItems();
		}, .2f, false);
	}
	else
	{
		OnLootGrantCompleted.Broadcast();
	}
}

void ULootSpawnComponent::InstantiateLootActors_Implementation(TArray<FLootInstance>& OutLootInstances)
{
	TArray<FLootDefinition> ToSpawn;
	FLootDefinition::ToShouldSpawn(LootDefinitions, ToSpawn);
	TArray<FTransform> Transforms = UElectricCastleSpawnBlueprintLibrary::GenerateSpawnLocations(
		GetOwner()->GetActorLocation(),
		SpawnDistance,
		GoldAmount > 0 ? ToSpawn.Num() + 1 : ToSpawn.Num(),
		120.f,
		GetOwner()->GetActorForwardVector(),
		GetOwner()->GetActorUpVector()
	);
	UArrayUtils::ShuffleArray(Transforms);
	const FTransform InitialTransform = GetInitialLootSpawnTransform();
	UElectricCastleGameDataSubsystem* GameDataSubsystem = UElectricCastleGameDataSubsystem::Get(this);
	for (int32 LootDefinitionIndex = 0; LootDefinitionIndex < ToSpawn.Num(); LootDefinitionIndex++)
	{
		const FLootDefinition& LootDefinition = ToSpawn[LootDefinitionIndex];
		const FTransform& TargetTransform = Transforms[LootDefinitionIndex];
		const FItemDefinition ItemDefinition = GameDataSubsystem->FindItemDefinitionByItemTag(LootDefinition.ItemTag);
		if (!ItemDefinition.IsValid())
		{
			UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Invalid item definition for item: [%s]"), *GetName(), *LootDefinition.ItemTag.ToString());
			continue;
		}
		if (AActor* Item = GetWorld()->SpawnActorDeferred<AActor>(
			ItemDefinition.ItemClass,
			InitialTransform,
			nullptr,
			nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn)
		)
		{
			int32 ItemLevel = LootDefinition.bOverrideOwnerLevel ? LootDefinition.Level : IElectricCastleAbilitySystemInterface::GetCharacterLevel(GetOwner());
			ITieredItemInterface::SetItemLevel(Item, ItemLevel);
			if (RewardAnimatorClass)
			{
				AActor* Animator = ISpawnEffectInterface::CreateSpawnEffectActor(this, RewardAnimatorClass, Item, true);
				Animator->SetOwner(GetOwner());
				Item->FinishSpawning(InitialTransform);
				OutLootInstances.Add(FLootInstance(Animator, InitialTransform, TargetTransform));
			}
			else
			{
				OutLootInstances.Add(FLootInstance(Item, InitialTransform, TargetTransform));
			}
		}
	}
	if (GoldAmount > 0)
	{
		const FTransform TargetTransform = RewardAnimatorClass ? Transforms[Transforms.Num() - 1] : InitialTransform;
		if (ATreasurePickup* GoldActor = ATreasurePickup::SpawnTreasure(GetOwner(), InitialTransform.GetLocation(), GoldAmount, false))
		{
			if (RewardAnimatorClass)
			{
				AActor* Animator = ISpawnEffectInterface::CreateSpawnEffectActor(this, RewardAnimatorClass, GoldActor, true);
				Animator->SetOwner(GetOwner());
				OutLootInstances.Add(FLootInstance(Animator, InitialTransform, TargetTransform));
			}
			else
			{
				OutLootInstances.Add(FLootInstance(GoldActor, TargetTransform));
			}
		}
	}
}

void ULootSpawnComponent::PlaySpawnItem_Implementation(const FLootInstance& LootInstance)
{
	if (LootInstance.LootItem.IsValid())
	{
		TArray<AActor*> ItemChildActors;
		AActor* LootItemActor = LootInstance.LootItem.Get();
		LootItemActor->SetActorHiddenInGame(false);
		LootItemActor->GetAttachedActors(ItemChildActors);
		for (AActor* ChildActor : ItemChildActors)
		{
			ChildActor->SetActorHiddenInGame(false);
		}
		if (LootItemActor->Implements<USpawnEffectInterface>())
		{
			ISpawnEffectInterface::PlaySpawnEffect(LootItemActor, LootInstance.SpawnTransform, LootInstance.TargetTransform, HeightMultiplier);
		}
		else
		{
			LootItemActor->SetActorLocation(LootInstance.TargetTransform.GetLocation());
			LootItemActor->SetActorRotation(LootInstance.TargetTransform.GetRotation());
		}
	}
}
