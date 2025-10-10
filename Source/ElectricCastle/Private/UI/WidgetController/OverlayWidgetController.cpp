// Copyright Alien Shores


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/ElectricCastleAbilitySystemComponent.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "AbilitySystem/ElectricCastleAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Game/Subsystem/ElectricCastleGameDataSubsystem.h"
#include "Player/ElectricCastlePlayerState.h"
#include "Player/InventoryComponent.h"
#include "Player/Form/FormChangeActorInterface.h"
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
	OnHealthChanged.Broadcast(FAuraFloatAttributeChangedPayload::CreateBroadcastPayload(AuraGameplayTags.Attributes_Vital_Health, GetAttributeSet()->GetHealth()));
	OnMaxMaxHealthChanged.Broadcast(FAuraFloatAttributeChangedPayload::CreateBroadcastPayload(AuraGameplayTags.Attributes_Primary_MaxHealth, GetAttributeSet()->GetMaxHealth()));
	OnManaChanged.Broadcast(FAuraFloatAttributeChangedPayload::CreateBroadcastPayload(AuraGameplayTags.Attributes_Vital_Mana, GetAttributeSet()->GetMana()));
	OnMaxManaChanged.Broadcast(FAuraFloatAttributeChangedPayload::CreateBroadcastPayload(AuraGameplayTags.Attributes_Primary_MaxMana, GetAttributeSet()->GetMaxMana()));
	if (const UProgressionComponent* ProgressionComponent = UProgressionComponent::Get(GetPlayerState()))
	{
		const float NewPercentage = UElectricCastleGameDataSubsystem::Get(GetPlayerState())->GetXPToNextLevelPercentage(ProgressionComponent->GetXP());
		OnXPPercentageChanged.Broadcast(FAuraFloatAttributeChangedPayload::CreateBroadcastPayload(AuraGameplayTags.Attributes_Progression_XP, NewPercentage));
		OnPlayerLevelChangedDelegate.Broadcast(FAuraIntAttributeChangedPayload::CreateBroadcastPayload(AuraGameplayTags.Attributes_Progression_Level, ProgressionComponent->GetCharacterLevel()));
	}
	if (GetAbilitySystemComponent()->HasFiredOnAbilitiesGivenDelegate())
	{
		BroadcastAbilityInfo();
	}
	else
	{
		GetAbilitySystemComponent()->OnAbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::BroadcastAbilityInfo);
	}
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();
	if (true)
	{
		return;
	}
	const FElectricCastleGameplayTags& GameplayTags = FElectricCastleGameplayTags::Get();
	if (UProgressionComponent* ProgressionComponent = UProgressionComponent::Get(GetPlayerState()))
	{
		ProgressionComponent->OnXPChangeDelegate.AddDynamic(this, &UOverlayWidgetController::OnPlayerXPChange);
		ProgressionComponent->OnLevelInitializedDelegate.AddDynamic(
			this,
			&UOverlayWidgetController::OnPlayerLevelInitialized
		);
		ProgressionComponent->OnLevelChangeDelegate.AddDynamic(this, &UOverlayWidgetController::OnPlayerLevelChange);
	}
	if (UPlayerFormChangeComponent* FormChangeComponent = IFormChangeActorInterface::GetFormChangeComponent(GetAuraPlayerController()->GetPawn()))
	{
		FormChangeComponent->OnPlayerFormChange.AddDynamic(this, &UOverlayWidgetController::OnFormChange);
	}
	if (AbilitySystemComponent)
	{
		if (AttributeSet)
		{
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAttributeSet()->GetHealthAttribute())
			                      .AddLambda(
				                      [&](const FOnAttributeChangeData& Data)
				                      {
					                      OnHealthChanged.Broadcast(FAuraFloatAttributeChangedPayload(GameplayTags.Attributes_Vital_Health, Data.OldValue, Data.NewValue));
				                      }
			                      );
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAttributeSet()->GetMaxHealthAttribute())
			                      .AddLambda(
				                      [&](const FOnAttributeChangeData& Data)
				                      {
					                      OnMaxMaxHealthChanged.Broadcast(FAuraFloatAttributeChangedPayload(GameplayTags.Attributes_Primary_MaxHealth, Data.OldValue, Data.NewValue));
				                      }
			                      );

			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAttributeSet()->GetManaAttribute())
			                      .AddLambda(
				                      [&](const FOnAttributeChangeData& Data)
				                      {
					                      OnManaChanged.Broadcast(FAuraFloatAttributeChangedPayload(GameplayTags.Attributes_Vital_Mana, Data.OldValue, Data.NewValue));
				                      }
			                      );
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAttributeSet()->GetMaxManaAttribute())
			                      .AddLambda(
				                      [&](const FOnAttributeChangeData& Data)
				                      {
					                      OnMaxManaChanged.Broadcast(FAuraFloatAttributeChangedPayload(GameplayTags.Attributes_Primary_MaxMana, Data.OldValue, Data.NewValue));
				                      }
			                      );
		}


		AbilitySystemComponent->OnEffectAssetTagsDelegate.AddLambda(
			[this](const FGameplayTagContainer& AssetTags)
			{
				for (const FGameplayTag& Tag : AssetTags)
				{
					FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
					if (Tag.MatchesTag(MessageTag))
					{
						if (const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageDataTable, Tag))
						{
							MessageWidgetRowDelegate.Broadcast(*Row, FMessageSubstitutions());
						}
					}
				}
			}
		);
		AbilitySystemComponent->OnAbilityEquippedDelegate.AddDynamic(this, &UOverlayWidgetController::OnAbilityEquipped);
		AbilitySystemComponent->RegisterGameplayTagEvent(
			FElectricCastleGameplayTags::Get().Player_HUD_Hide,
			EGameplayTagEventType::NewOrRemoved
		).AddUObject(this, &UOverlayWidgetController::OnPlayerHideHUDTagChanged);
	}
	if (UInventoryComponent* PlayerInventoryComponent = UInventoryComponent::Get(GetPlayerState()))
	{
		PlayerInventoryComponent->OnInventoryItemCountChangedDelegate.AddDynamic(this, &UOverlayWidgetController::OnPlayerInventoryChanged);
		PlayerInventoryComponent->OnInventoryFullDelegate.AddDynamic(this, &UOverlayWidgetController::OnPlayerInventoryFull);
	}
}

bool UOverlayWidgetController::IsBroadcastPayload(const FAuraIntAttributeChangedPayload& Payload)
{
	return !Payload.IsChanged();
}

void UOverlayWidgetController::OnPlayerXPChange(const FAuraIntAttributeChangedPayload& Payload)
{
	const float OldPercentage = UElectricCastleGameDataSubsystem::Get(GetPlayerState())->GetXPToNextLevelPercentage(Payload.OldValue);
	const float NewPercentage = UElectricCastleGameDataSubsystem::Get(GetPlayerState())->GetXPToNextLevelPercentage(Payload.NewValue);
	OnXPPercentageChanged.Broadcast(FAuraFloatAttributeChangedPayload(FElectricCastleGameplayTags::Get().Attributes_Progression_XP, OldPercentage, NewPercentage));
}

void UOverlayWidgetController::OnPlayerLevelInitialized(const FAuraIntAttributeChangedPayload& Payload)
{
	OnPlayerLevelInitializedDelegate.Broadcast(Payload);
}

void UOverlayWidgetController::OnPlayerLevelChange(const FAuraIntAttributeChangedPayload& Payload)
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
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Could not get game data subsystem for Player [%s]"), *GetName(), *Player->GetName())
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
		MessageTag = ItemDefinition.PickupMessageTag.IsValid() ? ItemDefinition.PickupMessageTag : GameDataSubsystem->GetDefaultItemPickupMessageTag();
	}
	else if (Payload.IsItemUsedChange())
	{
		MessageTag = ItemDefinition.UseMessageTag.IsValid() ? ItemDefinition.UseMessageTag : GameDataSubsystem->GetDefaultItemUseMessageTag();
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
