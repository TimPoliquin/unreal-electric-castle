// Copyright Alien Shores


#include "Fishing/FishingComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Fishing/FishInfo.h"
#include "Game/ElectricCastleGameState.h"
#include "Game/Subsystem/ElectricCastleGameDataSubsystem.h"
#include "Item/Equipment/FishingRodActor.h"
#include "Player/PlayerEquipmentComponent.h"
#include "Player/InventoryComponent.h"
#include "Tags/ElectricCastleGameplayTags.h"


UFishingComponent::UFishingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFishingComponent::SetPlayerEquipmentComponent(UPlayerEquipmentComponent* InPlayerEquipmentComponent)
{
	PlayerEquipmentComponent = InPlayerEquipmentComponent;
}

void UFishingComponent::SetupForFishing(const FVector& InFishingDestination)
{
	FishingDestination = InFishingDestination;
	if (IsValid(PlayerEquipmentComponent))
	{
		PlayerEquipmentComponent->OnEquipmentAnimationCompleteDelegate.AddUniqueDynamic(
			this,
			&UFishingComponent::OnInventoryEquipAnimationComplete
		);
	}
	else
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] No player inventory component set! Unable to bind equipment animation completion delegate."), *GetOwner()->GetName(), *GetName())
	}
	if (!FishingRestore.bSet)
	{
		FishingRestore.UseMode = PlayerEquipmentComponent->GetEquipmentUseMode();
		FishingRestore.WeaponType = PlayerEquipmentComponent->GetWeaponType();
		FishingRestore.ToolType = PlayerEquipmentComponent->GetToolType();
		FishingRestore.bSet = true;
	}
}

bool UFishingComponent::HasFishingRod() const
{
	if (const UInventoryComponent* PlayerInventoryComponent = UInventoryComponent::Get(GetOwner()))
	{
		return PlayerInventoryComponent->HasItemInInventory(FElectricCastleGameplayTags::Get().Item_Type_Equipment_FishingRod);
	}
	UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] No player inventory component!"), *GetOwner()->GetName(), *GetName())
	return false;
}

bool UFishingComponent::HasFishingRodEquipped() const
{
	if (!PlayerEquipmentComponent)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] No player inventory component set!"), *GetOwner()->GetName(), *GetName())
		return false;
	}
	return PlayerEquipmentComponent->HasToolEquipped(FElectricCastleGameplayTags::Get().Item_Type_Equipment_FishingRod);
}

void UFishingComponent::EquipFishingRod()
{
	if (!PlayerEquipmentComponent)
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[%s:%s] No PlayerEquipmentComponent set!"), *GetOwner()->GetName(), *GetName())
		return;
	}
	if (PlayerEquipmentComponent->HasToolEquipped(FElectricCastleGameplayTags::Get().Item_Type_Equipment_FishingRod))
	{
		OnFishingRodEquipped();
	}
	else
	{
		PlayerEquipmentComponent->Equip(
			EEquipmentSlot::Tool,
			FElectricCastleGameplayTags::Get().Item_Type_Equipment_FishingRod
		);
		PlayerEquipmentComponent->PlayEquipAnimation(EEquipmentSlot::Tool);
	}
}

void UFishingComponent::CastFishingRod()
{
	if (!PlayerEquipmentComponent)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] No player inventory component set!"), *GetOwner()->GetName(), *GetName())
		return;
	}
	SetFishingState(EFishingState::Casting);
	OnFishingComponentCastAnimationDelegate.Broadcast();
}

void UFishingComponent::FishStateChanged(const EFishState& FishState)
{
	switch (FishState)
	{
	case EFishState::None:

		break;
	case EFishState::Lured:
		SetFishingState(EFishingState::Lured);
		FishingBob->Lured();
		break;
	case EFishState::Biting:
		SetFishingState(EFishingState::Biting);
		FishingBob->Biting();
		break;
	case EFishState::Fighting:
		// DEVNOTE - nothing to do here - yet.
		break;
	case EFishState::Caught:
		SetFishingState(EFishingState::Caught);
		if (FishingRod)
		{
			FishingRod->Return();
		}
		if (FishingBob)
		{
			FishingBob->OnFishingStateChanged.RemoveDynamic(this, &UFishingComponent::OnFishingBobStateChanged);
		}
		break;
	case EFishState::Fled:
		SetFishingState(EFishingState::Fled);
		if (FishingRod)
		{
			FishingRod->Return();
		}
		if (FishingBob)
		{
			FishingBob->OnFishingStateChanged.RemoveDynamic(this, &UFishingComponent::OnFishingBobStateChanged);
		}
		break;
	}
}

void UFishingComponent::Reel()
{
	SetFishingState(EFishingState::Reeling);
}

FOnFishingStateChangedSignature& UFishingComponent::GetOnFishingStateChangedDelegate()
{
	return OnFishingStateChangedDelegate;
}

void UFishingComponent::PrepareForContinue()
{
	SetFishingState(EFishingState::Preparing);
	OnFishingStateChangedDelegate.Clear();
}

void UFishingComponent::EndFishing()
{
	if (FishingState == EFishingState::None)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("Fishing has already ended"));
		return;
	}
	SetFishingState(EFishingState::None);
	FishingDestination = FVector::ZeroVector;
	OnFishingStateChangedDelegate.Clear();
	if (FishingRod)
	{
		// TODO - clear fishing rod data
		FishingRod = nullptr;
	}
	if (FishingBob)
	{
		FishingBob->OnFishingStateChanged.RemoveDynamic(this, &UFishingComponent::OnFishingBobStateChanged);
		FishingBob->Cancel();
		FishingBob = nullptr;
	}
	switch (FishingRestore.UseMode)
	{
	case EEquipmentUseMode::None:
		PlayerEquipmentComponent->PlayEquipAnimation(EEquipmentSlot::None);
		break;
	case EEquipmentUseMode::Tool:
		PlayerEquipmentComponent->Equip(EEquipmentSlot::Tool, FishingRestore.ToolType);
		PlayerEquipmentComponent->PlayEquipAnimation(EEquipmentSlot::Tool);
		break;
	case EEquipmentUseMode::Weapon:
		PlayerEquipmentComponent->Equip(EEquipmentSlot::Weapon, FishingRestore.WeaponType);
		PlayerEquipmentComponent->PlayEquipAnimation(EEquipmentSlot::Weapon);
	}
	FishingRestore.Reset();
}

void UFishingComponent::ReleaseCast()
{
	if (FishingRod)
	{
		SetFishingState(EFishingState::Casting);
		FishingRod->Cast(FishingDestination);
	}
	else
	{
		UE_LOG(
			LogElectricCastle,
			Warning,
			TEXT("UFishingComponent::CastFishingRod - Attempted to release cast before rod has been equipped")
		);
	}
}

bool UFishingComponent::IsFishing() const
{
	return FishingState > EFishingState::None;
}

EFishingState UFishingComponent::GetFishingState() const
{
	return FishingState;
}

float UFishingComponent::GetRarityMultiplier(const FGameplayTag& Rarity) const
{
	const int32 FishingLevel = UElectricCastleAbilitySystemLibrary::GetAbilityLevelByAbilityTag(
		GetOwner(),
		FElectricCastleGameplayTags::Get().Abilities_Fishing
	);
	return UElectricCastleGameDataSubsystem::Get(GetOwner())->GetFishInfo()->
	                                                          GetFishRarityMultiplierByPlayerFishingLevel(
		                                                          FishingLevel,
		                                                          Rarity
	                                                          );
}

FGameplayTagContainer UFishingComponent::GetFishingTags() const
{
	if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
		GetOwner()
	))
	{
		FGameplayTagContainer FishingTags;
		FishingTags.AppendMatchingTags(
			AbilitySystemComponent->GetOwnedGameplayTags(),
			FElectricCastleGameplayTags::Get().Fish_Tag.GetSingleTagContainer()
		);
		return FishingTags;
	}
	return FGameplayTagContainer();
}

void UFishingComponent::SetFishingState(EFishingState InFishingState)
{
	if (InFishingState != FishingState)
	{
		FishingState = InFishingState;
		OnFishingStateChangedDelegate.Broadcast(InFishingState);
	}
	else
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("AuraFishingComponent: Potential loop detected setting state"));
	}
}

void UFishingComponent::OnFishingRodEquipped()
{
	SetFishingState(EFishingState::Equipped);
	FishingRod = PlayerEquipmentComponent->GetFishingRod();
	if (IsValid(FishingRod))
	{
		FishingBob = FishingRod->GetFishingBob();
	}
	if (IsValid(FishingBob))
	{
		FishingBob->OnFishingStateChanged.AddDynamic(this, &UFishingComponent::OnFishingBobStateChanged);
	}
	PlayerEquipmentComponent->OnEquipmentAnimationCompleteDelegate.RemoveDynamic(
		this,
		&UFishingComponent::OnInventoryEquipAnimationComplete
	);
}

void UFishingComponent::OnInventoryEquipAnimationComplete(const FEquipmentDelegatePayload& Payload)
{
	if (Payload.EquipmentSlot == EEquipmentSlot::Tool && Payload.EquipmentTag.MatchesTagExact(FElectricCastleGameplayTags::Get().Item_Type_Equipment_FishingRod))
	{
		OnFishingRodEquipped();
	}
}

void UFishingComponent::OnFishingBobStateChanged(EFishingBobState FishingBobState)
{
	switch (FishingBobState)
	{
	case EFishingBobState::Bobbing:
		SetFishingState(EFishingState::Cast);
		break;
	default:
		break;
	}
}
