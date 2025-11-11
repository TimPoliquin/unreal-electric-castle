// Copyright Alien Shores


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AbilitySystem/ElectricCastleAbilitySystemComponent.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "AbilitySystem/ElectricCastleAbilitySystemTypes.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Player/ElectricCastlePlayerState.h"
#include "Player/Progression/ProgressionComponent.h"
#include "Tags/ElectricCastleGameplayTags.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	if (!GetAbilitySystemComponent())
	{
		return;
	}
	if (GetAbilitySystemComponent()->HasFiredOnAbilitiesGivenDelegate())
	{
		BroadcastAbilityInfo();
	}
	else
	{
		GetAbilitySystemComponent()->OnAbilitiesGivenDelegate.AddLambda(
			[this]()
			{
				BroadcastAbilityInfo();
			}
		);
	}
	if (const UProgressionComponent* ProgressionComponent = UProgressionComponent::Get(GetPlayerState()))
	{
		OnSpellPointsChanged(
			FIntAttributeChangedPayload::CreateBroadcastPayload(
				FElectricCastleGameplayTags::Get().Attributes_Progression_SpellPoints,
				ProgressionComponent->GetSpellPoints()
			)
		);
	}
}


void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();
	GetAbilitySystemComponent()->OnPlayerLevelChangedDelegate.AddDynamic(
		this,
		&USpellMenuWidgetController::OnPlayerLevelChanged
	);
	GetAbilitySystemComponent()->OnAbilityEquippedDelegate.AddDynamic(
		this,
		&USpellMenuWidgetController::OnAbilityEquipped
	);
	if (UProgressionComponent* ProgressionComponent = UProgressionComponent::Get(GetPlayerState()))
	{
		ProgressionComponent->OnSpellPointsChangeDelegate.AddDynamic(
			this,
			&USpellMenuWidgetController::OnSpellPointsChanged
		);
	}
}

int32 USpellMenuWidgetController::GetAvailableSpellPoints()
{
	if (const UProgressionComponent* ProgressionComponent = UProgressionComponent::Get(GetPlayerState()))
	{
		return ProgressionComponent->GetSpellPoints();
	}
	return -1;
}

FGameplayTag USpellMenuWidgetController::GetAbilityStatusTag(const FGameplayTag AbilityTag)
{
	return UElectricCastleAbilitySystemLibrary::GetStatusTagByAbilityTag(GetAbilitySystemComponent(), AbilityTag);
}

bool USpellMenuWidgetController::HasAvailableSpellPoints()
{
	return GetAvailableSpellPoints() > 0;
}

bool USpellMenuWidgetController::CanEquipAbility(const FGameplayTag& AbilityTag)
{
	return UElectricCastleAbilitySystemLibrary::CanEquipAbility(GetAbilitySystemComponent(), AbilityTag);
}

bool USpellMenuWidgetController::CanPurchaseAbility(const FGameplayTag& AbilityTag)
{
	if (HasAvailableSpellPoints())
	{
		const FGameplayTag StatusTag = GetAbilityStatusTag(AbilityTag);
		FGameplayTagContainer PurchasableStatuses;
		PurchasableStatuses.AddTag(FElectricCastleGameplayTags::Get().Abilities_Status_Eligible);
		PurchasableStatuses.AddTag(FElectricCastleGameplayTags::Get().Abilities_Status_Unlocked);
		PurchasableStatuses.AddTag(FElectricCastleGameplayTags::Get().Abilities_Status_Equipped);
		return StatusTag.MatchesAny(PurchasableStatuses);
	}
	return false;
}

void USpellMenuWidgetController::SpendPointOnAbility(const FGameplayTag& AbilityTag)
{
	GetAbilitySystemComponent()->ServerSpendSpellPoint(AbilityTag);
}

FElectricCastleAbilityDescription USpellMenuWidgetController::GetAbilityDescription(const FGameplayTag AbilityTag)
{
	FElectricCastleAbilityDescription AbilityDescription;
	GetAbilitySystemComponent()->GetDescriptionsByAbilityTag(AbilityTag, AbilityDescription);
	return AbilityDescription;
}

FGameplayTag USpellMenuWidgetController::GetAbilityTypeTag(const FGameplayTag& AbilityTag) const
{
	return AbilityInfo->FindAbilityInfoForTag(AbilityTag).AbilityType;
}

FGameplayTag USpellMenuWidgetController::GetAbilityInputTag(const FGameplayTag AbilityTag)
{
	return UElectricCastleAbilitySystemLibrary::GetInputTagByAbilityTag(GetAbilitySystemComponent(), AbilityTag);
}

void USpellMenuWidgetController::EquipAbility(
	const FGameplayTag& AbilityTag,
	const FGameplayTag& SlotTag,
	const FGameplayTag& SelectedAbilityTypeTag
)
{
	const FGameplayTag AbilityType = GetAbilityTypeTag(AbilityTag);
	if (!SelectedAbilityTypeTag.MatchesTagExact(AbilityType))
	{
		UE_LOG(
			LogElectricCastle,
			Warning,
			TEXT("Attempted to assign an ability to an invalid slot. [%s] x-> [%s]"),
			*AbilityTag.ToString(),
			*SlotTag.ToString()
		)
		return;
	}
	GetAbilitySystemComponent()->ServerEquipAbility(
		AbilityTag,
		SlotTag
	);
}

void USpellMenuWidgetController::UnbindAll_Implementation(const UObject* BoundObject)
{
	Super::UnbindAll_Implementation(BoundObject);
	OnSpellMenuSpellPointsChangedDelegate.RemoveAll(BoundObject);
	OnSpellMenuPlayerLevelChangedDelegate.RemoveAll(BoundObject);
	OnSpellMenuSlotClearedDelegate.RemoveAll(BoundObject);
	OnAbilityAssigned.RemoveAll(BoundObject);
}


void USpellMenuWidgetController::OnSpellPointsChanged(const FIntAttributeChangedPayload& Payload)
{
	OnSpellMenuSpellPointsChangedDelegate.Broadcast(Payload);
}

void USpellMenuWidgetController::OnPlayerLevelChanged(
	const int32 Level,
	const TArray<FAbilityTagStatus>& AbilityStatuses
)
{
	if (AbilityInfo)
	{
		for (const FAbilityTagStatus& AbilityStatus : AbilityStatuses)
		{
			FElectricCastleAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityStatus.AbilityTag);
			Info.StatusTag = AbilityStatus.StatusTag;
			AbilityInfoDelegate.Broadcast(Info);
		}
	}
	OnSpellMenuPlayerLevelChangedDelegate.Broadcast(
		FIntAttributeChangedPayload(FElectricCastleGameplayTags::Get().Attributes_Progression_Level, Level, Level)
	);
}

void USpellMenuWidgetController::OnAbilityEquipped(const FElectricCastleEquipAbilityPayload& EquipPayload)
{
	// clear the previously occupied slot
	OnSpellMenuSlotClearedDelegate.Broadcast(EquipPayload.PreviousSlotTag);

	FElectricCastleAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(EquipPayload.AbilityTag);
	Info.StatusTag = EquipPayload.StatusTag;
	Info.InputTag = EquipPayload.SlotTag;
	AbilityInfoDelegate.Broadcast(Info);
	OnAbilityAssigned.Broadcast(EquipPayload.AbilityTag);
}
