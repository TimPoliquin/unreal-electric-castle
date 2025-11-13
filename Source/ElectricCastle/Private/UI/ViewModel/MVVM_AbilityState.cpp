// Copyright Alien Shores


#include "UI/ViewModel/MVVM_AbilityState.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/ElectricCastleAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Game/Subsystem/ElectricCastleGameDataSubsystem.h"
#include "GameFramework/PlayerState.h"

void UMVVM_AbilityState::InitializeDependencies(APlayerState* PlayerState)
{
	if (UElectricCastleAbilitySystemComponent* AbilitySystemComponent = Cast<UElectricCastleAbilitySystemComponent>(
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
			PlayerState
		)
	))
	{
		AbilitySystemComponent->OnAbilityAdded.AddUniqueDynamic(this, &UMVVM_AbilityState::OnPlayerAbilityAdded);
		AbilitySystemComponent->OnAbilityRemoved.AddUniqueDynamic(this, &UMVVM_AbilityState::OnPlayerAbilityRemoved);
	}
}

bool UMVVM_AbilityState::IsActive() const
{
	return AbilityTag.IsValid();
}

FGameplayTag UMVVM_AbilityState::GetAbilityTag() const
{
	return AbilityTag;
}

void UMVVM_AbilityState::SetAbilityTag(const FGameplayTag& InAbilityTag)
{
	UE_MVVM_SET_PROPERTY_VALUE(AbilityTag, InAbilityTag);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(IsActive);
}

UTexture2D* UMVVM_AbilityState::GetAbilityIcon() const
{
	return AbilityIcon;
}

void UMVVM_AbilityState::SetAbilityIcon(UTexture2D* InAbilityIcon)
{
	UE_MVVM_SET_PROPERTY_VALUE(AbilityIcon, InAbilityIcon);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetAbilityIconBrush);
}

FSlateBrush UMVVM_AbilityState::GetAbilityIconBrush() const
{
	FSlateBrush Brush;
	Brush.SetResourceObject(GetAbilityIcon());
	return Brush;
}

FGameplayTag UMVVM_AbilityState::GetInputTag() const
{
	return InputTag;
}

void UMVVM_AbilityState::SetInputTag(const FGameplayTag& InInputTag)
{
	UE_MVVM_SET_PROPERTY_VALUE(InputTag, InInputTag);
}

float UMVVM_AbilityState::GetCooldown_RemainingTime() const
{
	return Cooldown_RemainingTime;
}

void UMVVM_AbilityState::SetCooldown_RemainingTime(const float InCooldown_RemainingTime)
{
	UE_MVVM_SET_PROPERTY_VALUE(Cooldown_RemainingTime, InCooldown_RemainingTime);
}

float UMVVM_AbilityState::GetCooldown_TotalTime() const
{
	return Cooldown_TotalTime;
}

void UMVVM_AbilityState::SetCooldown_TotalTime(const float InCooldown_TotalTime)
{
	UE_MVVM_SET_PROPERTY_VALUE(Cooldown_TotalTime, InCooldown_TotalTime);
}

bool UMVVM_AbilityState::GetIsCoolingDown() const
{
	return IsCoolingDown;
}

void UMVVM_AbilityState::SetIsCoolingDown(const bool bIsCoolingDown)
{
	UE_MVVM_SET_PROPERTY_VALUE(IsCoolingDown, bIsCoolingDown);
}

void UMVVM_AbilityState::StartCooldown(float InDuration)
{
	UE_MVVM_SET_PROPERTY_VALUE(Cooldown_RemainingTime, InDuration);
	UE_MVVM_SET_PROPERTY_VALUE(Cooldown_TotalTime, InDuration);
	UE_MVVM_SET_PROPERTY_VALUE(IsCoolingDown, true);
}

void UMVVM_AbilityState::UpdateCooldown(const float DeltaTime)
{
	if (!IsCoolingDown)
	{
		return;
	}
	UE_MVVM_SET_PROPERTY_VALUE(Cooldown_RemainingTime, FMath::Max(Cooldown_RemainingTime - DeltaTime, 0.0f));
	if (Cooldown_RemainingTime <= 0.0f)
	{
		ClearCooldown();
	}
}

UMaterialInterface* UMVVM_AbilityState::GetAbilityBackground() const
{
	return AbilityBackground;
}

void UMVVM_AbilityState::SetAbilityBackground(UMaterialInterface* InAbilityBackground)
{
	UE_MVVM_SET_PROPERTY_VALUE(AbilityBackground, InAbilityBackground);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetAbilityBackgroundBrush);
}

FSlateBrush UMVVM_AbilityState::GetAbilityBackgroundBrush() const
{
	FSlateBrush Brush;
	Brush.SetResourceObject(GetAbilityBackground());
	return Brush;
}

void UMVVM_AbilityState::ClearCooldown()
{
	SetCooldown_RemainingTime(0.0f);
	SetCooldown_TotalTime(0.0f);
	SetIsCoolingDown(false);
}

void UMVVM_AbilityState::OnPlayerAbilityAdded(const FOnAbilityChangedPayload& Payload)
{
	UE_LOG(
		LogElectricCastle,
		Warning,
		TEXT("[%s][%s] Received ability added notice: %s|%s"),
		Payload.Owner->HasAuthority() ? *FString("Server") : *FString("Client"),
		*GetName(),
		*Payload.InputTag.ToString(),
		*Payload.AbilityTag.ToString()
	)
	if (Payload.InputTag.MatchesTagExact(InputTag))
	{
		UE_LOG(
			LogElectricCastle,
			Warning,
			TEXT("[%s] Ability matches %s - updating UI"),
			*GetName(),
			*InputTag.ToString()
		)
		AbilityTag = Payload.AbilityTag;
		if (const UElectricCastleGameDataSubsystem* GameData = UElectricCastleGameDataSubsystem::Get(Payload.Owner))
		{
			const FElectricCastleAbilityInfo& AbilityInfo = GameData->GetAbilityInfo()->FindAbilityInfoForTag(
				AbilityTag
			);
			SetAbilityTag(AbilityTag);
			SetAbilityIcon(AbilityInfo.Icon);
			SetAbilityBackground(AbilityInfo.BackgroundMaterial);
		}
		else
		{
			UE_LOG(LogElectricCastle, Warning, TEXT("[%s] No GameData Subsystem available!"), *GetName())
		}
	}
}

void UMVVM_AbilityState::OnPlayerAbilityRemoved(const FOnAbilityChangedPayload& Payload)
{
	if (Payload.InputTag.MatchesTagExact(Payload.InputTag))
	{
		SetAbilityTag(FGameplayTag::EmptyTag);
		SetAbilityIcon(nullptr);
		SetAbilityBackground(nullptr);
	}
}
