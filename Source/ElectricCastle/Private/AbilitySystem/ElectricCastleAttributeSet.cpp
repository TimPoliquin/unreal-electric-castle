// Copyright Alien Shores


#include "AbilitySystem/ElectricCastleAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystem/ElectricCastleAbilitySystemComponent.h"
#include "AbilitySystem/ElectricCastleAbilitySystemInterface.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/PlayerInterface.h"
#include "Net/UnrealNetwork.h"
#include "Player/ElectricCastlePlayerController.h"
#include "Tags/ElectricCastleGameplayTags.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"
#include "Player/Progression/ProgressionComponent.h"


UElectricCastleAttributeSet::UElectricCastleAttributeSet()
{
	const FElectricCastleGameplayTags& GameplayTags = FElectricCastleGameplayTags::Get();
	// Add primary attributes to attribute map
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_MaxHealth, GetMaxHealthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_MaxMana, GetMaxManaAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Strength, GetStrengthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Agility, GetAgilityAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Intelligence, GetIntelligenceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Constitution, GetConstitutionAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Wisdom, GetWisdomAttribute);
	// Add secondary attributes to attribute map
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_AttackPower, GetAttackPowerAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MagicPower, GetMagicPowerAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Defense, GetDefenseAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MagicDefense, GetMagicDefenseAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_HitChance, GetHitChanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_EvadeChance, GetEvadeChanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitChance, GetCriticalHitChanceAttribute);
}

void UElectricCastleAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// primary attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UElectricCastleAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UElectricCastleAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UElectricCastleAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UElectricCastleAttributeSet, Agility, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UElectricCastleAttributeSet, Constitution, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UElectricCastleAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UElectricCastleAttributeSet, Wisdom, COND_None, REPNOTIFY_Always);
	// secondary attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UElectricCastleAttributeSet, AttackPower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UElectricCastleAttributeSet, MagicPower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UElectricCastleAttributeSet, Defense, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UElectricCastleAttributeSet, MagicDefense, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UElectricCastleAttributeSet, HitChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UElectricCastleAttributeSet, EvadeChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UElectricCastleAttributeSet, CriticalHitChance, COND_None, REPNOTIFY_Always);
	// vital attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UElectricCastleAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UElectricCastleAttributeSet, Mana, COND_None, REPNOTIFY_Always);
}

void UElectricCastleAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	// This does not actually clamp the health value - it only clamps the modifier.
	// Attribute must also be clamped in PostAttributeChange.
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	else if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
}

void UElectricCastleAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	FEffectProperties Props;
	SetEffectProperties(Data, Props);
	if (ICombatInterface::IsDead(Props.Target.Character))
	{
		return;
	}
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}
	else if (Data.EvaluatedData.Attribute == GetMeta_IncomingDamageAttribute())
	{
		HandleIncomingDamage(Props);
	}
	else if (Data.EvaluatedData.Attribute == GetMeta_IncomingXPAttribute())
	{
		HandleIncomingXP(Props);
	}
	else if (Data.EvaluatedData.Attribute == GetMeta_IncomingRefreshAttribute())
	{
		HandleIncomingRefresh(Props);
	}
	if (UElectricCastleAbilitySystemLibrary::IsSuccessfulDebuff(Props.EffectContextHandle))
	{
		HandleDebuff(Props);
	}
}

void UElectricCastleAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
}

bool UElectricCastleAttributeSet::IsFullHealth() const
{
	return GetHealth() >= GetMaxHealth();
}

bool UElectricCastleAttributeSet::IsFullMana() const
{
	return GetMana() >= GetMaxMana();
}

void UElectricCastleAttributeSet::InitializeDefaultAttributes(const int32 Level)
{
	UElectricCastleAbilitySystemLibrary::ApplyBasicGameplayEffect(GetOwningActor(), DefaultPrimaryAttributes, Level);
	UElectricCastleAbilitySystemLibrary::ApplyBasicGameplayEffect(GetOwningActor(), DefaultSecondaryAttributes, Level);
	UElectricCastleAbilitySystemLibrary::ApplyBasicGameplayEffect(GetOwningActor(), InitializeVitalAttributes, Level);
}

void UElectricCastleAttributeSet::ToSaveData(FAttributeSetSaveData& SaveData) const
{
	SaveData.MaxHealth = GetMaxHealth();
	SaveData.MaxMana = GetMaxMana();
	SaveData.Health = GetHealth();
	SaveData.Mana = GetMana();
	SaveData.Strength = GetStrength();
	SaveData.Agility = GetAgility();
	SaveData.Constitution = GetConstitution();
	SaveData.Intelligence = GetIntelligence();
	SaveData.Wisdom = GetWisdom();
}

void UElectricCastleAttributeSet::FromSaveData(const FAttributeSetSaveData& SaveData)
{
	SetMaxHealth(SaveData.MaxHealth);
	SetMaxMana(SaveData.MaxMana);
	SetHealth(SaveData.Health);
	SetMana(SaveData.Mana);
	SetStrength(SaveData.Strength);
	SetAgility(SaveData.Agility);
	SetConstitution(SaveData.Constitution);
	SetIntelligence(SaveData.Intelligence);
	SetWisdom(SaveData.Wisdom);
}

void UElectricCastleAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
	Props.EffectContextHandle = Data.EffectSpec.GetContext();
	Props.Source.AbilitySystemComponent = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();
	if (Props.Source.AbilitySystemComponent)
	{
		Props.Source.AvatarActor = Props.Source.AbilitySystemComponent->GetAvatarActor();
		Props.Source.Controller = Props.Source.AbilitySystemComponent->AbilityActorInfo->PlayerController.Get();
	}
	if (Props.Source.Controller == nullptr && Props.Source.AvatarActor != nullptr)
	{
		if (const APawn* Pawn = Cast<APawn>(Props.Source.AvatarActor))
		{
			Props.Source.Controller = Pawn->GetController();
		}
	}
	if (Props.Source.Controller)
	{
		Props.Source.Character = Props.Source.Controller->GetCharacter();
	}
	if (Data.Target.AbilityActorInfo)
	{
		Props.Target.AvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.Target.Controller = Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.Target.Character = Cast<ACharacter>(Props.Target.AvatarActor);
		Props.Target.AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
			Props.Target.AvatarActor
		);
	}
}

void UElectricCastleAttributeSet::HandleIncomingDamage(const FEffectProperties& Props)
{
	const float IncomingDamage = GetMeta_IncomingDamage();
	SetMeta_IncomingDamage(0.f);
	if (IncomingDamage > 0.f)
	{
		HandleOutgoingDamage(Props, IncomingDamage);
		const float NewHealth = GetHealth() - IncomingDamage;
		const bool bFatal = NewHealth <= 0.f;
		SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));
		if (!bFatal)
		{
			if (UElectricCastleAbilitySystemLibrary::IsCriticalHit(Props.EffectContextHandle))
			{
				FGameplayTagContainer TagContainer;
				TagContainer.AddTag(
					ICombatInterface::GetHitReactAbilityTagByDamageType(
						Props.Target.AvatarActor,
						UElectricCastleAbilitySystemLibrary::GetDamageTypeTag(Props.EffectContextHandle)
					)
				);
				Props.Target.AbilitySystemComponent->TryActivateAbilitiesByTag(
					TagContainer
				);
			}
			if (const FVector KnockbackForce = UElectricCastleAbilitySystemLibrary::GetKnockbackVector(Props.EffectContextHandle);
				!
				KnockbackForce.IsNearlyZero(1.f))
			{
				Props.Target.Character->LaunchCharacter(KnockbackForce, true, true);
			}
		}
		else
		{
			if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Props.Target.AvatarActor))
			{
				CombatInterface->Die();
				CombatInterface->ApplyDeathImpulse(
					UElectricCastleAbilitySystemLibrary::GetDeathImpulse(Props.EffectContextHandle)
				);
			}
			SendXPEvent(Props);
		}
		ShowDamageText(Props, IncomingDamage);
	}
}

void UElectricCastleAttributeSet::HandleIncomingXP(const FEffectProperties& Props)
{
	const float IncomingXP = GetMeta_IncomingXP();
	SetMeta_IncomingXP(0.f);
	if (IPlayerInterface::ImplementsPlayerInterface(Props.Source.AvatarActor))
	{
		const int32 CurrentXP = IPlayerInterface::Execute_GetXP(Props.Source.Character);
		const int32 NewLevel = IPlayerInterface::Execute_FindLevelForXP(Props.Source.Character, CurrentXP + IncomingXP);
		const int32 CurrentLevel = IElectricCastleAbilitySystemInterface::GetCharacterLevel(Props.Source.Character);
		if (CurrentLevel < NewLevel)
		{
			for (int32 Idx = 0; Idx < NewLevel - CurrentLevel; Idx++)
			{
				// Get level up rewards (attribute & spell points)
				const FLevelUpRewards& Rewards = IPlayerInterface::Execute_GetLevelUpRewards(
					Props.Source.Character,
					CurrentLevel + Idx
				);
				// Apply rewards
				IPlayerInterface::Execute_ApplyLevelUpRewards(
					Props.Source.Character,
					1,
					Rewards
				);
			}
			if (UElectricCastleAbilitySystemComponent* AbilitySystemComponent = Cast<UElectricCastleAbilitySystemComponent>(
				Props.Source.AbilitySystemComponent
			))
			{
				AbilitySystemComponent->ServerUpdateAbilityStatuses(NewLevel);
			}
			SendReplenishVitalsEvent(Props.Source.AvatarActor);
			IPlayerInterface::Execute_LevelUp(Props.Source.Character);
		}
		IPlayerInterface::Execute_AddToXP(Props.Source.Character, IncomingXP);
	}
}

void UElectricCastleAttributeSet::HandleIncomingRefresh(const FEffectProperties& Props)
{
	const float IncomingRefresh = GetMeta_IncomingRefresh();
	SetMeta_IncomingRefresh(0.f);
	if (IncomingRefresh > 0.f)
	{
		SetHealth(GetMaxHealth());
		SetMana(GetMaxMana());
	}
}

void UElectricCastleAttributeSet::HandleDebuff(const FEffectProperties& Props)
{
	const FElectricCastleGameplayTags& GameplayTags = FElectricCastleGameplayTags::Get();
	FGameplayEffectContextHandle EffectContext = Props.Source.AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(Props.Source.AvatarActor);

	const FGameplayTag DebuffTypeTag = UElectricCastleAbilitySystemLibrary::GetDebuffTypeTag(Props.EffectContextHandle);
	// TODO - get debuff gameplay effect from a data lookup rather than trying to calculate it here in C++.
	// const float DebuffDamage = UElectricCastleAbilitySystemLibrary::GetDebuffDamage(Props.EffectContextHandle);
	// const float DebuffDuration = UElectricCastleAbilitySystemLibrary::GetDebuffDuration(Props.EffectContextHandle);
	// const float DebuffFrequency = UElectricCastleAbilitySystemLibrary::GetDebuffFrequency(Props.EffectContextHandle);
	//
	// const FString DebuffName = FString::Printf(TEXT("DynamicDebuff_%s"), *DebuffTypeTag.ToString());
	// UGameplayEffect* Effect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(DebuffName));
	//
	// Effect->DurationPolicy = EGameplayEffectDurationType::HasDuration;
	// Effect->Period = DebuffFrequency;
	// Effect->DurationMagnitude = FScalableFloat(DebuffDuration);
	//
	// Effect->StackingType = EGameplayEffectStackingType::AggregateBySource;
	// Effect->StackLimitCount = 1;
	//
	// const int32 Index = Effect->Modifiers.Num();
	// Effect->Modifiers.Add(FGameplayModifierInfo());
	// FGameplayModifierInfo& ModifierInfo = Effect->Modifiers[Index];
	//
	// ModifierInfo.ModifierMagnitude = FScalableFloat(DebuffDamage);
	// ModifierInfo.ModifierOp = EGameplayModOp::Additive;
	// ModifierInfo.Attribute = GetMeta_IncomingDamageAttribute();
	//
	// UTargetTagsGameplayEffectComponent& TagsComponent = Effect->FindOrAddComponent<
	// 	UTargetTagsGameplayEffectComponent>();
	// FInheritedTagContainer InheritedTagContainer = FInheritedTagContainer();
	// InheritedTagContainer.Added.AddTag(DebuffTypeTag);
	// if (DebuffTypeTag.MatchesTagExact(GameplayTags.Effect_Debuff_Type_Shock))
	// {
	// 	InheritedTagContainer.Added.AddTag(FElectricCastleGameplayTags::Get().Player_Block_CursorTrace);
	// 	InheritedTagContainer.Added.AddTag(FElectricCastleGameplayTags::Get().Player_Block_Interaction);
	// 	InheritedTagContainer.Added.AddTag(FElectricCastleGameplayTags::Get().Player_Block_Movement);
	// 	InheritedTagContainer.Added.AddTag(FElectricCastleGameplayTags::Get().Player_Block_Ability_Offensive);
	// }
	// TagsComponent.SetAndApplyTargetTagChanges(InheritedTagContainer);
	//
	// if (const FGameplayEffectSpec* MutableSpec = new FGameplayEffectSpec(Effect, EffectContext, 1.f))
	// {
	// 	FElectricCastleGameplayEffectContext* AuraContext = static_cast<FElectricCastleGameplayEffectContext*>(MutableSpec->GetContext().
	// 	                                                                                                                    Get());
	// 	const TSharedPtr<FGameplayTag> DebuffDamageType = MakeShareable(new FGameplayTag(DamageTypeTag));
	// 	AuraContext->SetDamageTypeTag(DebuffDamageType);
	//
	// 	Props.Target.AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*MutableSpec);
	// }
}

void UElectricCastleAttributeSet::HandleOutgoingDamage(const FEffectProperties& Props, const float IncomingDamage)
{
	if (const UElectricCastleAbilitySystemComponent* SourceAbilitySystemComponent = Cast<UElectricCastleAbilitySystemComponent>(
		Props.Source.AbilitySystemComponent
	))
	{
		SourceAbilitySystemComponent->OnOutgoingDamageDelegate.Broadcast(
			FOnAbilitySystemOutgoingDamagePayload(Props.Source.AvatarActor, Props.Target.AvatarActor, IncomingDamage)
		);
	}
}


void UElectricCastleAttributeSet::ShowDamageText(const FEffectProperties& Props, const float& IncomingDamage) const
{
	if (!Props.Source.Character || !Props.Target.Character)
	{
		return;
	}
	if (Props.Source.Character != Props.Target.Character)
	{
		if (AElectricCastlePlayerController* PlayerController = Cast<AElectricCastlePlayerController>(
			Props.Source.Character->GetController()
		))
		{
			// show damage on enemy
			PlayerController->ShowDamageNumber(
				Props.Target.Character,
				IncomingDamage,
				UElectricCastleAbilitySystemLibrary::IsBlockedHit(Props.EffectContextHandle),
				UElectricCastleAbilitySystemLibrary::IsCriticalHit(Props.EffectContextHandle)
			);
		}
		else if (AElectricCastlePlayerController* TargetPlayerController = Cast<AElectricCastlePlayerController>(
			Props.Target.Character->GetController()
		))
		{
			// show damage on player
			TargetPlayerController->ShowDamageNumber(
				Props.Target.Character,
				IncomingDamage,
				UElectricCastleAbilitySystemLibrary::IsBlockedHit(Props.EffectContextHandle),
				UElectricCastleAbilitySystemLibrary::IsCriticalHit(Props.EffectContextHandle)
			);
		}
	}
}

void UElectricCastleAttributeSet::SendXPEvent(const FEffectProperties& Props) const
{
	const int32 XPReward = ICombatInterface::GetXPReward(Props.Target.AvatarActor);
	FGameplayEventData Payload;
	Payload.EventTag = FElectricCastleGameplayTags::Get().Attributes_Meta_IncomingXP;
	Payload.EventMagnitude = XPReward;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		Props.Source.AvatarActor,
		Payload.EventTag,
		Payload
	);
}

void UElectricCastleAttributeSet::SendReplenishVitalsEvent(AActor* AvatarActor)
{
	FGameplayEventData Payload;
	Payload.EventTag = FElectricCastleGameplayTags::Get().Attributes_Meta_IncomingRefresh;
	Payload.EventMagnitude = 1;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		AvatarActor,
		Payload.EventTag,
		Payload
	);
}
