// Copyright Alien Shores

#include "Character/Status/StatusEffectManagerComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/ElectricCastleAbilitySystemInterface.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "Character/Status/StatusEffectConfig.h"
#include "Game/Subsystem/ElectricCastleGameDataSubsystem.h"

UStatusEffectManagerComponent::UStatusEffectManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UStatusEffectManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	if (IElectricCastleAbilitySystemInterface::IsAbilitySystemReady(GetOwner()))
	{
		AddStatusEffectListeners(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()));
	} else if (IElectricCastleAbilitySystemInterface* AbilitySystemInterface = Cast<IElectricCastleAbilitySystemInterface>(GetOwner()))
	{
		AbilitySystemInterface->GetOnAbilitySystemRegisteredDelegate().AddUObject(this, &UStatusEffectManagerComponent::AddStatusEffectListeners);
	}
}

void UStatusEffectManagerComponent::AddStatusEffectListeners(UAbilitySystemComponent* AbilitySystemComponent)
{
	AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(this, &UStatusEffectManagerComponent::OnEffectAdded);
	AbilitySystemComponent->OnAnyGameplayEffectRemovedDelegate().AddUObject(this, &UStatusEffectManagerComponent::OnEffectRemoved);
}

void UStatusEffectManagerComponent::OnEffectAdded(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle) const
{
	const UGameplayEffect* GameplayEffect = Spec.Def;
	const UElectricCastleGameDataSubsystem* GameDataSubsystem = UElectricCastleGameDataSubsystem::Get(GetOwner());
	const UStatusEffectConfig* StatusEffectConfig = GameDataSubsystem ? GameDataSubsystem->GetStatusEffectConfig() : nullptr;
	if (!GameplayEffect) return;
	if (!StatusEffectConfig) return;

	// Extract tags
	const FGameplayTagContainer StatusEffectTags = StatusEffectConfig->GetMatchingStatusEffectTags(GameplayEffect->GetGrantedTags());
	if (StatusEffectTags.IsValid())
	{
		// Duration vs Infinite
		const float Duration = Spec.GetDuration(); // -1 for infinite
		const float DurationRemaining = AbilitySystemComponent->GetActiveGameplayEffect(Handle)->GetTimeRemaining(GetWorld()->GetTimeSeconds());
		const bool bIsInfinite = Duration < 0.f;
		// Broadcast to UI
		OnStatusEffectAddedDelegate.Broadcast(FOnStatusEffectAddedPayload(
			GetOwner(), 
			UElectricCastleAbilitySystemLibrary::GetAbilitySystemComponent(GetOwner()), 
			StatusEffectTags, 
			Duration,
			DurationRemaining,
			bIsInfinite
		));
		if (bDebug)
		{
			UE_LOG(LogTemp, Warning, TEXT("[%s] Status Effect Added: %s"), *GetName(), *StatusEffectTags.ToString());
		}
	}
}

void UStatusEffectManagerComponent::OnEffectRemoved(const FActiveGameplayEffect& ActiveGameplayEffect) const
{
	const UGameplayEffect* GameplayEffect = ActiveGameplayEffect.Spec.Def;
	const UElectricCastleGameDataSubsystem* GameDataSubsystem = UElectricCastleGameDataSubsystem::Get(GetOwner());
	const UStatusEffectConfig* StatusEffectConfig = GameDataSubsystem ? GameDataSubsystem->GetStatusEffectConfig() : nullptr;
	if (!GameplayEffect) return;
	if (!StatusEffectConfig) return;

	// Extract tags
	const FGameplayTagContainer StatusEffectTags = StatusEffectConfig->GetMatchingStatusEffectTags(GameplayEffect->GetGrantedTags());
	if (StatusEffectTags.IsValid())
	{
		OnStatusEffectRemovedDelegate.Broadcast(FOnStatusEffectRemovedPayload(
			GetOwner(), 
			UElectricCastleAbilitySystemLibrary::GetAbilitySystemComponent(GetOwner()), 
			StatusEffectTags
		));
		if (bDebug)
		{
			UE_LOG(LogTemp, Warning, TEXT("[%s] Status Effect Removed: %s"), *GetName(), *StatusEffectTags.ToString());
		}
	}
}
