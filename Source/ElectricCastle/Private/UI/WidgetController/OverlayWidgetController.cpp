// Copyright Alien Shores


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/ElectricCastleAbilitySystemComponent.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "AbilitySystem/ElectricCastleAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Game/Subsystem/ElectricCastleGameDataSubsystem.h"
#include "Player/ElectricCastlePlayerState.h"
#include "Player/InventoryEvents.h"
#include "Player/Form/FormConfigLoadRequest.h"
#include "Player/Form/PlayerFormChangeComponent.h"
#include "Player/Form/PlayerFormConfig.h"
#include "Player/Progression/ProgressionComponent.h"
#include "Tags/ElectricCastleGameplayTags.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	if (!GetAttributeSet())
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s] No attribute set provided!"), *GetName());
		return;
	}
	const FElectricCastleGameplayTags& AuraGameplayTags = FElectricCastleGameplayTags::Get();
	OnHealthChanged.Broadcast(
		FFloatAttributeChangedPayload::CreateBroadcastPayload(
			AuraGameplayTags.Attributes_Vital_Health,
			GetAttributeSet()->GetHealth()
		)
	);
	OnMaxMaxHealthChanged.Broadcast(
		FFloatAttributeChangedPayload::CreateBroadcastPayload(
			AuraGameplayTags.Attributes_Primary_MaxHealth,
			GetAttributeSet()->GetMaxHealth()
		)
	);
	OnManaChanged.Broadcast(
		FFloatAttributeChangedPayload::CreateBroadcastPayload(
			AuraGameplayTags.Attributes_Vital_Mana,
			GetAttributeSet()->GetMana()
		)
	);
	OnMaxManaChanged.Broadcast(
		FFloatAttributeChangedPayload::CreateBroadcastPayload(
			AuraGameplayTags.Attributes_Primary_MaxMana,
			GetAttributeSet()->GetMaxMana()
		)
	);
	if (const UProgressionComponent* ProgressionComponent = UProgressionComponent::Get(GetPlayerState()))
	{
		const float NewPercentage = UElectricCastleGameDataSubsystem::Get(GetPlayerState())->GetXPToNextLevelPercentage(
			ProgressionComponent->GetXP()
		);
		OnXPPercentageChanged.Broadcast(
			FFloatAttributeChangedPayload::CreateBroadcastPayload(
				AuraGameplayTags.Attributes_Progression_XP,
				NewPercentage
			)
		);
		OnPlayerLevelChangedDelegate.Broadcast(
			FIntAttributeChangedPayload::CreateBroadcastPayload(
				AuraGameplayTags.Attributes_Progression_Level,
				ProgressionComponent->GetCharacterLevel()
			)
		);
	}
	if (GetAbilitySystemComponent()->HasFiredOnAbilitiesGivenDelegate())
	{
		BroadcastAbilityInfo();
	}
	else
	{
		GetAbilitySystemComponent()->OnAbilitiesGivenDelegate.AddUObject(
			this,
			&UOverlayWidgetController::BroadcastAbilityInfo
		);
	}
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();
	if constexpr (true)
	{
	}
}

bool UOverlayWidgetController::IsBroadcastPayload(const FIntAttributeChangedPayload& Payload)
{
	return !Payload.IsChanged();
}

void UOverlayWidgetController::OnPlayerXPChange(const FIntAttributeChangedPayload& Payload)
{
	const float OldPercentage = UElectricCastleGameDataSubsystem::Get(GetPlayerState())->GetXPToNextLevelPercentage(
		Payload.OldValue
	);
	const float NewPercentage = UElectricCastleGameDataSubsystem::Get(GetPlayerState())->GetXPToNextLevelPercentage(
		Payload.NewValue
	);
	OnXPPercentageChanged.Broadcast(
		FFloatAttributeChangedPayload(
			FElectricCastleGameplayTags::Get().Attributes_Progression_XP,
			OldPercentage,
			NewPercentage
		)
	);
}

void UOverlayWidgetController::OnPlayerLevelInitialized(const FIntAttributeChangedPayload& Payload)
{
	OnPlayerLevelInitializedDelegate.Broadcast(Payload);
}

void UOverlayWidgetController::OnPlayerLevelChange(const FIntAttributeChangedPayload& Payload)
{
	OnPlayerLevelChangedDelegate.Broadcast(Payload);
}

void UOverlayWidgetController::OnAbilityEquipped(const FElectricCastleEquipAbilityPayload& EquipPayload)
{
	// clear the previously occupied slot
	OnClearSlot.Broadcast(EquipPayload.PreviousSlotTag);

	FElectricCastleAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(EquipPayload.AbilityTag);
	Info.StatusTag = EquipPayload.StatusTag;
	Info.InputTag = EquipPayload.SlotTag;
	AbilityInfoDelegate.Broadcast(Info);
}

void UOverlayWidgetController::OnPlayerHideHUDTagChanged(FGameplayTag GameplayTag, int Count)
{
	UE_LOG(LogElectricCastle, Warning, TEXT("[%s]: %d"), *GameplayTag.ToString(), Count);
	OnHUDVisibilityChangedDelegate.Broadcast(Count == 0);
}

void UOverlayWidgetController::OnPlayerInventoryChanged(const FOnInventoryItemCountChangedPayload& Payload)
{
	UElectricCastleGameDataSubsystem* GameDataSubsystem = UElectricCastleGameDataSubsystem::Get(Player);
	if (!GameDataSubsystem)
	{
		UE_LOG(
			LogElectricCastle,
			Warning,
			TEXT("[%s] Could not get game data subsystem for Player [%s]"),
			*GetName(),
			*Player->GetName()
		)
		return;
	}
	const FItemDefinition ItemDefinition = GameDataSubsystem->FindItemDefinitionByItemTag(Payload.ItemType);
	FGameplayTag MessageTag;
	if (!ItemDefinition.IsValid())
	{
		// do nothing - invalid item type
		return;
	}
	if (Payload.IsItemAddedChange())
	{
		MessageTag = ItemDefinition.PickupMessageTag.IsValid()
			             ? ItemDefinition.PickupMessageTag
			             : GameDataSubsystem->GetDefaultItemPickupMessageTag();
	}
	else if (Payload.IsItemUsedChange())
	{
		MessageTag = ItemDefinition.UseMessageTag.IsValid()
			             ? ItemDefinition.UseMessageTag
			             : GameDataSubsystem->GetDefaultItemUseMessageTag();
	}
	if (!MessageTag.IsValid())
	{
		return;
	}
	if (const FUIWidgetRow* WidgetRow = GetDataTableRowByTag<FUIWidgetRow>(MessageDataTable, MessageTag))
	{
		FMessageSubstitutions Substitutions;
		Substitutions.Add(FString("ItemName"), ItemDefinition.ItemName);
		Substitutions.Add(FString("Count"), FString::FromInt(FMath::Abs(Payload.GetDelta())));
		Substitutions.Icon = ItemDefinition.ItemIcon;
		MessageWidgetRowDelegate.Broadcast(*WidgetRow, Substitutions);
	}
}

void UOverlayWidgetController::OnPlayerInventoryFull(const FGameplayTag& ItemType)
{
}

void UOverlayWidgetController::OnPortraitLoaded(const FPlayerFormConfigRow& Row)
{
	OnOverlayPortraitChangedDelegate.Broadcast(FOverlayPortraitChangedPayload(Row.PortraitImage.Get()));
}

void UOverlayWidgetController::OnFormChange(const FPlayerFormChangeEventPayload& Payload)
{
	if (const UElectricCastleGameDataSubsystem* GameData = UElectricCastleGameDataSubsystem::Get(GetPlayerState()))
	{
		if (UPlayerFormConfig* FormConfig = GameData->GetPlayerFormConfig())
		{
			UFormConfigLoadRequest* LoadRequest = FormConfig->GetOrCreateLoadRequest(Payload.NewFormTag);
			LoadRequest->OnLoadComplete.AddUniqueDynamic(this, &UOverlayWidgetController::OnPortraitLoaded);
			FormConfig->LoadAsync(LoadRequest);
		}
	}
}
