// Copyright Alien Shores


#include "UI/ViewModel/MVVM_PlayerState.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/ElectricCastleAbilitySystemComponent.h"
#include "AbilitySystem/ElectricCastleAttributeSet.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Player/ElectricCastlePlayerState.h"
#include "Player/Form/FormChangeActorInterface.h"
#include "Player/Form/FormConfigLoadRequest.h"
#include "Player/Progression/ProgressionComponent.h"

int32 UMVVM_PlayerState::GetPlayerIndex() const
{
	return PlayerIndex;
}

void UMVVM_PlayerState::SetPlayerIndex(const int32 InPlayerIndex)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerIndex, InPlayerIndex);
}

int32 UMVVM_PlayerState::GetLevel() const
{
	return Level;
}

void UMVVM_PlayerState::SetLevel(const int32 InLevel)
{
	UE_MVVM_SET_PROPERTY_VALUE(Level, InLevel);
}

float UMVVM_PlayerState::GetHealth() const
{
	return Health;
}

void UMVVM_PlayerState::SetHealth(const float InHealth)
{
	UE_MVVM_SET_PROPERTY_VALUE(Health, InHealth);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetHealthPercent);
	if (InHealth <= 0.0f)
	{
		IsDead = true;
	}
}

float UMVVM_PlayerState::GetMaxHealth() const
{
	return MaxHealth;
}

void UMVVM_PlayerState::SetMaxHealth(const float InMaxHealth)
{
	UE_MVVM_SET_PROPERTY_VALUE(MaxHealth, InMaxHealth);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetHealthPercent);
}

float UMVVM_PlayerState::GetHealthPercent() const
{
	if (MaxHealth > 0.0f)
	{
		return Health / MaxHealth;
	}
	return 0;
}

float UMVVM_PlayerState::GetMana() const
{
	return Mana;
}

void UMVVM_PlayerState::SetMana(const float InMana)
{
	UE_MVVM_SET_PROPERTY_VALUE(Mana, InMana);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetManaPercent);
}

float UMVVM_PlayerState::GetMaxMana() const
{
	return MaxMana;
}

void UMVVM_PlayerState::SetMaxMana(const float InMaxMana)
{
	UE_MVVM_SET_PROPERTY_VALUE(MaxMana, InMaxMana);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetManaPercent);
}

float UMVVM_PlayerState::GetManaPercent() const
{
	return MaxMana > 0.0f
		       ? Mana / MaxMana
		       : 0.0f;
}

int32 UMVVM_PlayerState::GetXP() const
{
	return XP;
}

void UMVVM_PlayerState::SetXP(const int32 InXP)
{
	UE_MVVM_SET_PROPERTY_VALUE(XP, InXP);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetXPPercent);
}

int32 UMVVM_PlayerState::GetXPMax() const
{
	return XPMax;
}

void UMVVM_PlayerState::SetXPMax(const int32 InXPMax)
{
	UE_MVVM_SET_PROPERTY_VALUE(XPMax, InXPMax);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetXPPercent);
}

float UMVVM_PlayerState::GetXPPercent() const
{
	return XPMax > 0
		       ? static_cast<float>(XP) / static_cast<float>(XPMax)
		       : 0.0f;
}

UTexture2D* UMVVM_PlayerState::GetPortraitIcon() const
{
	return PortraitIcon;
}

void UMVVM_PlayerState::SetPortraitIcon(UTexture2D* InPortraitIcon)
{
	UE_MVVM_SET_PROPERTY_VALUE(PortraitIcon, InPortraitIcon);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetPortraitBrush);
}

bool UMVVM_PlayerState::GetIsActive() const
{
	return IsActive;
}

void UMVVM_PlayerState::SetIsActive(const bool InActive)
{
	UE_MVVM_SET_PROPERTY_VALUE(IsActive, InActive);
}

bool UMVVM_PlayerState::GetIsDead() const
{
	return IsDead;
}

void UMVVM_PlayerState::SetIsDead(const bool InIsDead)
{
	UE_MVVM_SET_PROPERTY_VALUE(IsDead, InIsDead);
}

FSlateBrush UMVVM_PlayerState::GetPortraitBrush() const
{
	FSlateBrush Brush;
	Brush.SetResourceObject(GetPortraitIcon());
	return Brush;
}

void UMVVM_PlayerState::InitializeDependencies(const AElectricCastlePlayerState* PlayerState)
{
	if (!PlayerState)
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[%s] PlayerState is null"), *GetName());
		return;
	}
	if (!PlayerState->GetAbilitySystemComponent())
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[%s] PlayerState has no AbilitySystemComponent"), *GetName());
		return;
	}
	if (!PlayerState->GetAttributeSet())
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[%s] PlayerState has no AttributeSet"), *GetName());
		return;
	}
	// Setup Level and XP tracking
	if (UProgressionComponent* ProgressionComponent = UProgressionComponent::Get(PlayerState))
	{
		ProgressionComponent->OnXPChangeDelegate.AddDynamic(this, &UMVVM_PlayerState::OnPlayerXPChange);
		ProgressionComponent->OnLevelInitializedDelegate.AddDynamic(
			this,
			&UMVVM_PlayerState::OnPlayerLevelInitialized
		);
		ProgressionComponent->OnLevelChangeDelegate.AddDynamic(this, &UMVVM_PlayerState::OnPlayerLevelChange);
	}
	// Setup Form Change tracking
	if (UPlayerFormChangeComponent* FormChangeComponent = IFormChangeActorInterface::GetFormChangeComponent(
		PlayerState->GetPawn()
	))
	{
		FormChangeComponent->OnPlayerFormDataLoaded.AddDynamic(this, &UMVVM_PlayerState::OnFormChange);
	}
	// Setup Vital Attribute Tracking (Health, Mana)
	UElectricCastleAbilitySystemComponent* AbilitySystemComponent = PlayerState->
		GetElectricCastleAbilitySystemComponent();
	const UElectricCastleAttributeSet* AttributeSet = PlayerState->GetAttributeSet();
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute())
	                      .AddLambda(
		                      [&](const FOnAttributeChangeData& Data)
		                      {
			                      SetHealth(Data.NewValue);
		                      }
	                      );
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxHealthAttribute())
	                      .AddLambda(
		                      [&](const FOnAttributeChangeData& Data)
		                      {
			                      SetMaxHealth(Data.NewValue);
		                      }
	                      );
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetManaAttribute())
	                      .AddLambda(
		                      [&](const FOnAttributeChangeData& Data)
		                      {
			                      SetMana(Data.NewValue);
		                      }
	                      );
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxManaAttribute())
	                      .AddLambda(
		                      [&](const FOnAttributeChangeData& Data)
		                      {
			                      SetMaxMana(Data.NewValue);
		                      }
	                      );
}

void UMVVM_PlayerState::OnPlayerXPChange(const FIntAttributeChangedPayload& Payload)
{
	SetXP(Payload.NewValue);
}

void UMVVM_PlayerState::OnPlayerLevelInitialized(const FIntAttributeChangedPayload& Payload)
{
	SetLevel(Payload.NewValue);
}

void UMVVM_PlayerState::OnPlayerLevelChange(const FIntAttributeChangedPayload& Payload)
{
	SetLevel(Payload.NewValue);
}

void UMVVM_PlayerState::OnFormChange(const FPlayerFormConfigRow& Payload)
{
	SetPortraitIcon(Payload.PortraitImage.Get());
}
