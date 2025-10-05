// Copyright Alien Shores


#include "AbilitySystem/ElectricCastleAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "AbilitySystem/ElectricCastleAbilitySystemTypes.h"
#include "AbilitySystem/Ability/ElectricCastleGameplayAbility.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Game/Save/SaveGameTypes.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/PlayerInterface.h"
#include "Tags/ElectricCastleGameplayTags.h"


void UElectricCastleAbilitySystemComponent::ForEachAbility(const FForEachAbility& ForEachAbilityDelegate)
{
	// This locks the ability system abilities for the scope of this function call.
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!ForEachAbilityDelegate.ExecuteIfBound(AbilitySpec))
		{
			UE_LOG(LogElectricCastle, Error, TEXT("Failed to execute delegate in [%hs]"), __FUNCTION__);
		}
	}
}

void UElectricCastleAbilitySystemComponent::ServerUpdateAbilityStatuses(const int32 Level)
{
	const FGameplayTag& EligibleStatusTag = FElectricCastleGameplayTags::Get().Abilities_Status_Eligible;
	const UAbilityInfo* AbilityInfo = UElectricCastleAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	TArray<FAbilityTagStatus> EligibleAbilities;
	for (const FElectricCastleAbilityInfo& Info : AbilityInfo->AbilityInformation)
	{
		if (Level < Info.LevelRequirement || !Info.AbilityTag.IsValid())
		{
			continue;
		}
		if (GetSpecFromAbilityTag(Info.AbilityTag) == nullptr)
		{
			// TODO - this level is wrong. 
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Info.Ability, 1);
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(EligibleStatusTag);
			GiveAbility(AbilitySpec);
			// Force replication immediately
			MarkAbilitySpecDirty(AbilitySpec);
			// Broadcast to clients
			EligibleAbilities.Add(FAbilityTagStatus::Create(Info.AbilityTag, EligibleStatusTag, AbilitySpec.Level));
		}
	}
	ClientUpdateAbilityStatus(Level, EligibleAbilities);
}

void UElectricCastleAbilitySystemComponent::MulticastActivatePassiveEffect_Implementation(
	const FGameplayTag& AbilityTag,
	bool bActivate
)
{
	OnActivatePassiveEffectDelegate.Broadcast(AbilityTag, bActivate);
}

void UElectricCastleAbilitySystemComponent::ServerSpendSpellPoint_Implementation(const FGameplayTag& AbilityTag)
{
	if (IPlayerInterface::GetSpellPoints(GetAvatarActor()) <= 0)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("No spell points to spend points on ability [%s]"), *AbilityTag.ToString())
		return;
	}
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		IPlayerInterface::SpendSpellPoints(GetAvatarActor(), 1);
		const FElectricCastleGameplayTags GameplayTags = FElectricCastleGameplayTags::Get();
		if (const FGameplayTag StatusTag = UElectricCastleAbilitySystemLibrary::GetStatusTagFromSpec(*AbilitySpec);
			StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
		{
			// unlock ability
			AbilitySpec->GetDynamicSpecSourceTags().RemoveTag(GameplayTags.Abilities_Status_Eligible);
			AbilitySpec->GetDynamicSpecSourceTags().AddTag(GameplayTags.Abilities_Status_Unlocked);
		}
		else if (StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Equipped) || StatusTag.MatchesTagExact(
			GameplayTags.Abilities_Status_Unlocked
		))
		{
			// upgrade ability
			AbilitySpec->Level += 1;
			// TODO Cancel and reactivate the ability if it is passive
		}
		ClientUpdateAbilityStatus(
			IElectricCastleAbilitySystemInterface::GetCharacterLevel(GetAvatarActor()),
			FAbilityTagStatus::CreateArray(
				AbilityTag,
				UElectricCastleAbilitySystemLibrary::GetStatusTagFromSpec(*AbilitySpec),
				AbilitySpec->Level
			)
		);
		MarkAbilitySpecDirty(*AbilitySpec);
	}
}

void UElectricCastleAbilitySystemComponent::ServerEquipAbility_Implementation(
	const FGameplayTag& AbilityTag,
	const FGameplayTag& SlotTag
)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		const FElectricCastleGameplayTags GameplayTags = FElectricCastleGameplayTags::Get();
		const FGameplayTag PreviousSlot = UElectricCastleAbilitySystemLibrary::GetInputTagFromSpec(*AbilitySpec);
		if (UElectricCastleAbilitySystemLibrary::CanEquipAbility(this, AbilityTag))
		{
			if (FGameplayAbilitySpec* SpecWithSlot = GetAbilitySpecWithSlot(SlotTag))
			{
				// there is an ability in this slot already. Deactivate the ability and clear the slot.
				if (AbilityTag.MatchesTagExact(UElectricCastleAbilitySystemLibrary::GetAbilityTagFromSpec(*SpecWithSlot)))
				{
					// equipped the same ability in the same slot - early return.
					ClientEquipAbility(
						FElectricCastleEquipAbilityPayload::Create(
							AbilityTag,
							GameplayTags.Abilities_Status_Equipped,
							SlotTag,
							PreviousSlot
						)
					);
					return;
				}
				if (UElectricCastleAbilitySystemLibrary::IsPassiveAbility(GetAvatarActor(), *SpecWithSlot))
				{
					OnDeactivatePassiveAbilityDelegate.Broadcast(
						UElectricCastleAbilitySystemLibrary::GetAbilityTagFromSpec(*SpecWithSlot)
					);
					MulticastActivatePassiveEffect(
						UElectricCastleAbilitySystemLibrary::GetAbilityTagFromSpec(*SpecWithSlot),
						false
					);
				}
				ClearAbilitySlot(*SpecWithSlot);
			}
			if (!UElectricCastleAbilitySystemLibrary::AbilityHasAnySlot(*AbilitySpec))
			{
				AbilitySpec->GetDynamicSpecSourceTags().RemoveTag(
					UElectricCastleAbilitySystemLibrary::GetStatusTagFromSpec(*AbilitySpec)
				);
				AbilitySpec->GetDynamicSpecSourceTags().AddTag(GameplayTags.Abilities_Status_Equipped);
				// ability is not yet equipped/active
				if (UElectricCastleAbilitySystemLibrary::IsPassiveAbility(GetAvatarActor(), *AbilitySpec))
				{
					TryActivateAbility(AbilitySpec->Handle);
					MulticastActivatePassiveEffect(AbilityTag, true);
				}
			}
			AssignSlotTagToAbilitySpec(*AbilitySpec, SlotTag);
			MarkAbilitySpecDirty(*AbilitySpec);
			ClientEquipAbility(
				FElectricCastleEquipAbilityPayload::Create(
					AbilityTag,
					GameplayTags.Abilities_Status_Equipped,
					SlotTag,
					PreviousSlot
				)
			);
		}
	}
}

void UElectricCastleAbilitySystemComponent::ClientEquipAbility_Implementation(const FElectricCastleEquipAbilityPayload& EquipPayload)
{
	OnAbilityEquippedDelegate.Broadcast(EquipPayload);
}

void UElectricCastleAbilitySystemComponent::ClearAbilitySlot(FGameplayAbilitySpec& AbilitySpec)
{
	for (const FGameplayTag& Slot : UElectricCastleAbilitySystemLibrary::GetInputTagsFromSpec(AbilitySpec))
	{
		AbilitySpec.GetDynamicSpecSourceTags().RemoveTag(Slot);
	}
}

void UElectricCastleAbilitySystemComponent::ClearAbilitiesUsingSlot(const FGameplayTag& SlotTag)
{
	FForEachAbility ClearSlotDelegate;
	ClearSlotDelegate.BindLambda(
		[this, SlotTag](FGameplayAbilitySpec& AbilitySpec)
		{
			if (UElectricCastleAbilitySystemLibrary::AbilityHasSlotTag(AbilitySpec, SlotTag))
			{
				ClearAbilitySlot(AbilitySpec);
			}
		}
	);
	ForEachAbility(ClearSlotDelegate);
}

FGameplayAbilitySpec* UElectricCastleAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& AbilityTag)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		for (const FGameplayTag& Tag : AbilitySpec.Ability.Get()->GetAssetTags())
		{
			if (Tag.MatchesTagExact(AbilityTag))
			{
				return &AbilitySpec;
			}
		}
	}
	return nullptr;
}

bool UElectricCastleAbilitySystemComponent::GetDescriptionsByAbilityTag(
	const FGameplayTag& AbilityTag,
	FElectricCastleAbilityDescription& OutDescription
)
{
	if (!AbilityTag.MatchesTag(FElectricCastleGameplayTags::Get().Abilities))
	{
		return false;
	}
	if (const FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		if (UElectricCastleGameplayAbility* AuraAbility = Cast<UElectricCastleGameplayAbility>(AbilitySpec->Ability))
		{
			OutDescription.Description = AuraAbility->GetDescription(AbilitySpec->Level);
			OutDescription.NextLevelDescription = AuraAbility->GetDescription(AbilitySpec->Level + 1);
			return true;
		}
		UE_LOG(LogElectricCastle, Error, TEXT("Ability not set for AbilitySpec assigned to [%s]"), *AbilityTag.ToString())
	}
	const UAbilityInfo* AbilityInfo = UElectricCastleAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	OutDescription.Description = UElectricCastleGameplayAbility::GetLockedDescription(
		AbilityInfo->FindAbilityInfoForTag(AbilityTag).LevelRequirement
	);
	OutDescription.NextLevelDescription = FString();
	return false;
}

TArray<uint8> UElectricCastleAbilitySystemComponent::SaveData_Implementation()
{
	return SerializeActorComponent();
}

bool UElectricCastleAbilitySystemComponent::LoadData_Implementation(const TArray<uint8>& Data)
{
	return DeserializeActorComponent(Data);
}

bool UElectricCastleAbilitySystemComponent::ShouldSave_Implementation() const
{
	return bShouldSave;
}

void UElectricCastleAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(
		this,
		&UElectricCastleAbilitySystemComponent::Client_EffectApplied
	);
}

bool UElectricCastleAbilitySystemComponent::IsSlotEmpty(const FGameplayTag& SlotTag)
{
	bool HasTagExact = false;
	FForEachAbility Delegate;
	Delegate.BindLambda(
		[SlotTag, &HasTagExact](FGameplayAbilitySpec& AbilitySpec)
		{
			HasTagExact = UElectricCastleAbilitySystemLibrary::AbilityHasSlotTag(AbilitySpec, SlotTag);
		}
	);
	ForEachAbility(Delegate);
	return HasTagExact;
}

FGameplayAbilitySpec* UElectricCastleAbilitySystemComponent::GetAbilitySpecWithSlot(const FGameplayTag& SlotTag)
{
	FGameplayAbilitySpec* AbilityInSlot = nullptr;
	FForEachAbility Delegate;
	Delegate.BindLambda(
		[SlotTag, &AbilityInSlot](FGameplayAbilitySpec& AbilitySpec)
		{
			if (UElectricCastleAbilitySystemLibrary::AbilityHasSlotTag(AbilitySpec, SlotTag))
			{
				AbilityInSlot = &AbilitySpec;
			}
		}
	);
	ForEachAbility(Delegate);
	return AbilityInSlot;
}

void UElectricCastleAbilitySystemComponent::AssignSlotTagToAbilitySpec(
	FGameplayAbilitySpec& AbilitySpec,
	const FGameplayTag& SlotTag
)
{
	ClearAbilitySlot(AbilitySpec);
	AbilitySpec.GetDynamicSpecSourceTags().AddTag(SlotTag);
	if (SlotTag.MatchesTagExact(FElectricCastleGameplayTags::Get().InputTag_LeftMouseButton))
	{
		// This is a special tag that binds to Shift + Left Click.
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(FElectricCastleGameplayTags::Get().InputTag_AttackTarget);
	}
}

TArray<uint8> UElectricCastleAbilitySystemComponent::SerializeActorComponent()
{
	TArray<FSavedAbility> SavedAbilityInfo;
	const AActor* Actor = GetAvatarActor();
	const UAbilityInfo* AbilityInfo = UElectricCastleAbilitySystemLibrary::GetAbilityInfo(Actor);
	FForEachAbility SaveAbilityDelegate;
	SaveAbilityDelegate.BindLambda(
		[&](FGameplayAbilitySpec& AbilitySpec)
		{
			const FGameplayTag AbilityTag = UElectricCastleAbilitySystemLibrary::GetAbilityTagFromSpec(AbilitySpec);
			if (AbilityTag.IsValid())
			{
				const FElectricCastleAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
				FSavedAbility SavedAbility;
				SavedAbility.GameplayAbilityClass = Info.Ability;
				SavedAbility.AbilityLevel = AbilitySpec.Level;
				SavedAbility.AbilityTag = AbilityTag;
				SavedAbility.AbilityTypeTag = Info.AbilityType;
				SavedAbility.AbilitySlotTag = UElectricCastleAbilitySystemLibrary::GetInputTagFromSpec(AbilitySpec);
				SavedAbility.AbilityStatusTag = UElectricCastleAbilitySystemLibrary::GetStatusTagFromSpec(AbilitySpec);
				SavedAbility.AbilityState = AbilitySpec.ActiveCount > 0
					                            ? ESavedAbilityState::GiveAbilityAndActivate
					                            : ESavedAbilityState::GiveAbility;
				SavedAbilityInfo.Add(SavedAbility);
			}
			else
			{
				UE_LOG(LogElectricCastle, Warning, TEXT("No ability tag for ability: [%s]"), *AbilitySpec.Ability.GetName());
			}
		}
	);
	ForEachAbility(SaveAbilityDelegate);

	TArray<uint8> SaveData;
	FMemoryWriter Writer(SaveData, true);
	// Serialize the struct
	uint32 NumAbilities = SavedAbilityInfo.Num();
	Writer << NumAbilities;
	Writer << SavedAbilityInfo;
	return SaveData;
}

bool UElectricCastleAbilitySystemComponent::DeserializeActorComponent(const TArray<uint8>& Data)
{
	if (Data.Num() == 0)
	{
		return false;
	}

	FMemoryReader Reader(Data);

	try
	{
		uint32 NumAbilities = 0;
		TArray<FSavedAbility> SavedAbilityInfo;
		Reader << NumAbilities;
		Reader << SavedAbilityInfo;

		const FElectricCastleGameplayTags& GameplayTags = FElectricCastleGameplayTags::Get();
		for (const FSavedAbility& SavedAbility : SavedAbilityInfo)
		{
			const TSubclassOf<UGameplayAbility> AbilityClass = SavedAbility.GameplayAbilityClass;
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, SavedAbility.AbilityLevel);
			AssignSlotTagToAbilitySpec(AbilitySpec, SavedAbility.AbilitySlotTag);
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(SavedAbility.AbilityStatusTag);
			if (SavedAbility.AbilityStatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Equipped))
			{
				if (SavedAbility.AbilityState == ESavedAbilityState::GiveAbilityAndActivate)
				{
					GiveAbilityAndActivateOnce(AbilitySpec);
				}
				else
				{
					GiveAbility(AbilitySpec);
				}
			}
			else if (!SavedAbility.AbilityStatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Locked))
			{
				GiveAbility(AbilitySpec);
			}
			MarkAbilitySpecDirty(AbilitySpec);
		}
		bAbilitiesGiven = true;
		OnAbilitiesGivenDelegate.Broadcast();

		return true;
	}
	catch (...)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s:%s] Failed to deserialize data"), *GetOwner()->GetName(), *GetName());
		return false;
	}
}

void UElectricCastleAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(
	const int32 PlayerLevel,
	const TArray<FAbilityTagStatus>& AbilityStatuses
)
{
	OnPlayerLevelChangedDelegate.Broadcast(PlayerLevel, AbilityStatuses);
}

void UElectricCastleAbilitySystemComponent::Client_EffectApplied_Implementation(
	UAbilitySystemComponent* AbilitySystemComponent,
	const FGameplayEffectSpec& EffectSpec,
	FActiveGameplayEffectHandle ActiveEffectHandle
)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);
	OnEffectAssetTagsDelegate.Broadcast(TagContainer);
}

void UElectricCastleAbilitySystemComponent::AddCharacterAbilities(
	const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities,
	const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities
)
{
	for (const TSubclassOf AbilityClass : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		FGameplayTag EquippedTag = FElectricCastleGameplayTags::Get().Abilities_Status_Equipped;
		if (const UElectricCastleGameplayAbility* AuraAbility = Cast<UElectricCastleGameplayAbility>(AbilitySpec.Ability))
		{
			for (FGameplayTag StartupTag : AuraAbility->GetStartupInputTag())
			{
				AbilitySpec.GetDynamicSpecSourceTags().AddTag(StartupTag);
			}
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(EquippedTag);
			GiveAbility(AbilitySpec);
		}
	}
	for (const TSubclassOf PassiveAbilityClass : StartupPassiveAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(PassiveAbilityClass, 1);
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(FElectricCastleGameplayTags::Get().Abilities_Status_Equipped);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
	// NOTE: This is client-side only! OnRep_ActivateAbilities handles server-side.
	bAbilitiesGiven = true;
	OnAbilitiesGivenDelegate.Broadcast();
}

void UElectricCastleAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}
	FForEachAbility Delegate;
	Delegate.BindLambda(
		[this, InputTag](FGameplayAbilitySpec& AbilitySpec)
		{
			if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
			{
				AbilitySpecInputPressed(AbilitySpec);
				if (AbilitySpec.IsActive())
				{
					InvokeReplicatedEvent(
						EAbilityGenericReplicatedEvent::InputPressed,
						AbilitySpec.Handle,
						UElectricCastleAbilitySystemLibrary::GetPredictionKeyFromAbilitySpec(AbilitySpec)
					);
				}
			}
		}
	);
	ForEachAbility(Delegate);
}

void UElectricCastleAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}
	FForEachAbility Delegate;
	Delegate.BindLambda(
		[this, InputTag](FGameplayAbilitySpec& AbilitySpec)
		{
			if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
			{
				AbilitySpecInputPressed(AbilitySpec);
				if (!AbilitySpec.IsActive())
				{
					TryActivateAbility(AbilitySpec.Handle);
				}
			}
		}
	);
	ForEachAbility(Delegate);
}

void UElectricCastleAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}
	FForEachAbility Delegate;

	Delegate.BindLambda(
		[this, InputTag](FGameplayAbilitySpec& AbilitySpec)
		{
			if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag) && AbilitySpec.IsActive())
			{
				AbilitySpecInputReleased(AbilitySpec);
				InvokeReplicatedEvent(
					EAbilityGenericReplicatedEvent::InputReleased,
					AbilitySpec.Handle,
					UElectricCastleAbilitySystemLibrary::GetPredictionKeyFromAbilitySpec(AbilitySpec)
				);
			}
		}
	);
	ForEachAbility(Delegate);
}

void UElectricCastleAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();
	if (!bAbilitiesGiven)
	{
		bAbilitiesGiven = true;
		OnAbilitiesGivenDelegate.Broadcast();
	}
}
