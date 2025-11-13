// Copyright Alien Shores


#include "UI/ViewModel/MVVM_AbilityState.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/ElectricCastleAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Game/Subsystem/ElectricCastleGameDataSubsystem.h"
#include "GameFramework/PlayerState.h"
#include "Tags/ElectricCastleGameplayTags.h"

void UMVVM_AbilityState::InitializeDependencies(APlayerState* PlayerState)
{
	AbilitySystemComponent = Cast<UElectricCastleAbilitySystemComponent>(
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
			PlayerState
		)
	);
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->OnAbilityAdded.AddUniqueDynamic(this, &UMVVM_AbilityState::OnPlayerAbilityAdded);
		AbilitySystemComponent->OnAbilityRemoved.AddUniqueDynamic(this, &UMVVM_AbilityState::OnPlayerAbilityRemoved);
		AbilitySystemComponent->RegisterGameplayTagEvent(
			FElectricCastleGameplayTags::Get().Player_Block_Ability_Offensive,
			EGameplayTagEventType::NewOrRemoved
		).AddLambda(
			[this](const FGameplayTag& Tag, const int32 Count)
			{
				SetIsBlocked(Count > 0);
			}
		);
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

FGameplayTag UMVVM_AbilityState::GetCooldownTag() const
{
	return CooldownTag;
}

void UMVVM_AbilityState::SetCooldownTag(const FGameplayTag& InCooldownTag)
{
	const FGameplayTag& OldCooldownTag = CooldownTag;
	UE_MVVM_SET_PROPERTY_VALUE(CooldownTag, InCooldownTag);

	if (CooldownTag.IsValid())
	{
		CooldownDelegateHandle = AbilitySystemComponent->RegisterGameplayTagEvent(
			AbilityInfo.CooldownTag,
			EGameplayTagEventType::NewOrRemoved
		).AddUObject(this, &UMVVM_AbilityState::OnCooldownTagChanged);
		StartCooldown(FindActiveGameplayEffectWithCooldownTag(CooldownTag));
	}
	else if (CooldownDelegateHandle.IsValid())
	{
		AbilitySystemComponent->UnregisterGameplayTagEvent(
			CooldownDelegateHandle,
			OldCooldownTag,
			EGameplayTagEventType::NewOrRemoved
		);
		ClearCooldown();
	}
}

bool UMVVM_AbilityState::GetIsCoolingDown() const
{
	return IsCoolingDown;
}

void UMVVM_AbilityState::SetIsCoolingDown(const bool bIsCoolingDown)
{
	UE_MVVM_SET_PROPERTY_VALUE(IsCoolingDown, bIsCoolingDown);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetIsAbilityAvailable);
}

bool UMVVM_AbilityState::GetIsBlocked() const
{
	return IsBlocked;
}

void UMVVM_AbilityState::SetIsBlocked(const bool bIsBlocked)
{
	UE_MVVM_SET_PROPERTY_VALUE(IsBlocked, bIsBlocked);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetIsAbilityAvailable);
}

bool UMVVM_AbilityState::GetIsAbilityAvailable() const
{
	return !IsBlocked && !IsCoolingDown;
}

FElectricCastleAbilityInfo UMVVM_AbilityState::GetAbilityInfo() const
{
	return AbilityInfo;
}

void UMVVM_AbilityState::SetAbilityInfo(const FElectricCastleAbilityInfo& InAbilityInfo)
{
	AbilityInfo = InAbilityInfo;
	if (AbilityInfo.IsValid())
	{
		SetAbilityTag(AbilityTag);
		SetAbilityIcon(AbilityInfo.Icon);
		SetAbilityBackground(AbilityInfo.BackgroundMaterial);
		SetCooldownTag(AbilityInfo.CooldownTag);
	}
	else
	{
		SetAbilityTag(FGameplayTag::EmptyTag);
		SetAbilityIcon(nullptr);
		SetAbilityBackground(nullptr);
		SetCooldownTag(FGameplayTag::EmptyTag);
	}
}

void UMVVM_AbilityState::StartCooldown(float InDuration)
{
	SetCooldown_RemainingTime(InDuration);
	SetCooldown_TotalTime(InDuration);
	SetIsCoolingDown(true);
}

void UMVVM_AbilityState::StartCooldown(const FActiveGameplayEffectHandle& InCooldownEffectHandle)
{
	if (!InCooldownEffectHandle.IsValid())
	{
		return;
	}
	StartCooldown(
		AbilitySystemComponent->GetActiveGameplayEffect(InCooldownEffectHandle)->GetTimeRemaining(
			AbilitySystemComponent->GetWorld()->GetTimeSeconds()
		)
	);
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
	if (Payload.InputTag.MatchesTagExact(InputTag))
	{
		AbilityTag = Payload.AbilityTag;
		if (const UElectricCastleGameDataSubsystem* GameData = UElectricCastleGameDataSubsystem::Get(Payload.Owner))
		{
			SetAbilityInfo(
				GameData->GetAbilityInfo()->FindAbilityInfoForTag(
					AbilityTag
				)
			);
		}
		else
		{
			UE_LOG(LogElectricCastle, Warning, TEXT("[%s] No GameData Subsystem available!"), *GetName())
		}
	}
}

void UMVVM_AbilityState::OnPlayerAbilityRemoved(const FOnAbilityChangedPayload& Payload)
{
	if (Payload.InputTag.MatchesTagExact(InputTag))
	{
		SetAbilityInfo(FElectricCastleAbilityInfo());
	}
}

FActiveGameplayEffectHandle UMVVM_AbilityState::FindActiveGameplayEffectWithCooldownTag(
	const FGameplayTag InCooldownTag
) const
{
	TArray<FActiveGameplayEffectHandle> ActiveEffects = AbilitySystemComponent->GetActiveEffectsWithAllTags(
		InCooldownTag.GetSingleTagContainer()
	);
	for (const FActiveGameplayEffectHandle& Handle : ActiveEffects)
	{
		if (AbilitySystemComponent->GetActiveGameplayEffect(Handle))
		{
			return Handle;
		}
	}
	return FActiveGameplayEffectHandle();
}

void UMVVM_AbilityState::OnCooldownTagChanged(
	const FGameplayTag InCooldownTag,
	const int NewCount
)
{
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s] No AbilitySystemComponent set!"), *GetName())
		return;
	}

	if (NewCount > 0)
	{
		// Cooldown applied: find the active GE with this tag
		if (const FActiveGameplayEffectHandle& CooldownEffect = FindActiveGameplayEffectWithCooldownTag(InCooldownTag);
			CooldownEffect.IsValid())
		{
			StartCooldown(CooldownEffect);
		}
		else
		{
			ClearCooldown();
		}
	}
	else
	{
		ClearCooldown();
	}
}
