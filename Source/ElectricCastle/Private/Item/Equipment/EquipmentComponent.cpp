// Copyright Alien Shores


#include "Item/Equipment/EquipmentComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"


// Sets default values for this component's properties
UEquipmentComponent::UEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UEquipmentComponent::IsEquipped() const
{
	return EquipEffectHandle.IsValid();
}

FGameplayTag UEquipmentComponent::GetItemType() const
{
	return ItemType;
}

void UEquipmentComponent::SetItemType(const FGameplayTag InItemType)
{
	ItemType = InItemType;
}

FGameplayTag UEquipmentComponent::GetEquippedTag() const
{
	return EquippedTag;
}

void UEquipmentComponent::SetEquippedTag(const FGameplayTag InEquippedTag)
{
	EquippedTag = InEquippedTag;
}

void UEquipmentComponent::Equip(AActor* InOwner)
{
	if (IsEquipped())
	{
		return;
	}
	ApplyEquipEffect(InOwner);
}

void UEquipmentComponent::Unequip(AActor* InOwner)
{
	if (!IsEquipped())
	{
		return;
	}
	RemoveEquipEffect(InOwner);
}

void UEquipmentComponent::ApplyEquipEffect(AActor* InOwner)
{
	if (EquipEffectHandle.IsValid())
	{
		// This is an unexpected case. Let's unequip it and re-equip it.
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s][%s] Equip effect is already applied!"), *InOwner->GetName(), *GetName());
		return;
	}
	EquipEffectHandle = UElectricCastleAbilitySystemLibrary::ApplyInfiniteEffectByTag(InOwner, EquippedTag, 1);
}

void UEquipmentComponent::RemoveEquipEffect(AActor* InOwner)
{
	if (EquipEffectHandle.IsValid())
	{
		UElectricCastleAbilitySystemLibrary::RemoveGameplayEffect(InOwner, EquipEffectHandle);
		EquipEffectHandle.Invalidate();
	}
}
