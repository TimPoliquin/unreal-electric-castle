// Copyright Alien Shores

#include "Actor/Status/StatusEffectManagerComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "AbilitySystem/ElectricCastleAbilitySystemComponent.h"
#include "AbilitySystem/ElectricCastleAbilitySystemInterface.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "Actor/Status/Data/StatusEffectConfig.h"
#include "Game/Subsystem/ElectricCastleGameDataSubsystem.h"
#include "GameFramework/Character.h"
#include "UI/Widget/Status/StatusEffectBarWidget.h"

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
		AddStatusEffectListeners(UElectricCastleAbilitySystemLibrary::GetAbilitySystemComponent(GetOwner()));
	}
	else if (IElectricCastleAbilitySystemInterface* AbilitySystemInterface = Cast<IElectricCastleAbilitySystemInterface>(GetOwner()))
	{
		AbilitySystemInterface->GetOnAbilitySystemRegisteredDelegate().AddUObject(this, &UStatusEffectManagerComponent::AddStatusEffectListeners);
	}
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner()))
	{
		CombatInterface->GetOnDeathDelegate().AddUniqueDynamic(this, &UStatusEffectManagerComponent::HandleOwnerDeath);
	}
}

UStatusEffectBarWidget* UStatusEffectManagerComponent::GetStatusEffectBarWidget() const
{
	return StatusEffectBarWidget;
}

void UStatusEffectManagerComponent::SetStatusEffectBarWidget(UStatusEffectBarWidget* InStatusEffectBarWidget)
{
	StatusEffectBarWidget = InStatusEffectBarWidget;
}

void UStatusEffectManagerComponent::AddStatusEffectNiagaraSystem(const FGameplayTag& InStatusEffectTag, UNiagaraSystem* NiagaraSystem, const FStatusEffectNiagaraConfig& Config)
{
	if (!InStatusEffectTag.IsValid())
	{
		return;
	}
	if (!IsValid(NiagaraSystem))
	{
		return;
	}
	if (const ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		StatusEffectFXInstances.Add(
			FStatusEffectFXInstance(
				InStatusEffectTag,
				UNiagaraFunctionLibrary::SpawnSystemAttached(
					NiagaraSystem,
					// DEVNOTE: Attaching to the character mesh is perhaps an incomplete solution, as it does not include clothing or the head.
					Character->GetMesh(),
					Config.AttachPoint,
					Config.Location,
					Config.Rotation,
					Config.LocationType,
					true,
					true
				)
			)
		);
	}
}

void UStatusEffectManagerComponent::RemoveStatusEffectNiagaraSystem(const FGameplayTag& InStatusEffectTag)
{
	auto Predicate = [InStatusEffectTag](const FStatusEffectFXInstance& Instance)
	{
		return Instance.StatusEffectTag.MatchesTagExact(InStatusEffectTag);
	};
	if (const FStatusEffectFXInstance* FXInstance = StatusEffectFXInstances.FindByPredicate(Predicate); FXInstance && FXInstance->IsValid() && IsValid(FXInstance->FXInstance))
	{
		FXInstance->FXInstance->DestroyComponent();
	}
	StatusEffectFXInstances.RemoveAll(Predicate);
}

void UStatusEffectManagerComponent::AddStatusEffectListeners(UElectricCastleAbilitySystemComponent* AbilitySystemComponent)
{
	AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(this, &UStatusEffectManagerComponent::OnEffectAdded);
	AbilitySystemComponent->OnAnyGameplayEffectRemovedDelegate().AddUObject(this, &UStatusEffectManagerComponent::OnEffectRemoved);
}

void UStatusEffectManagerComponent::OnEffectAdded(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle) const
{
	const UElectricCastleGameDataSubsystem* GameDataSubsystem = UElectricCastleGameDataSubsystem::Get(GetOwner());
	const UStatusEffectConfig* StatusEffectConfig = GameDataSubsystem ? GameDataSubsystem->GetStatusEffectConfig() : nullptr;
	if (!StatusEffectConfig)
	{
		return;
	}

	// Extract tags
	FGameplayTagContainer SpecTags;
	Spec.GetAllGrantedTags(SpecTags);
	const FGameplayTagContainer StatusEffectTags = StatusEffectConfig->GetMatchingStatusEffectTags(SpecTags);
	if (const FStatusEffectRow& StatusEffectRow = StatusEffectConfig->FindStatusEffectRowByTags(SpecTags); StatusEffectRow.IsValid())
	{
		// Duration vs Infinite
		FStatusEffectDuration Duration;
		Duration.Duration = Spec.GetDuration(); // -1 for infinite
		Duration.DurationRemaining = AbilitySystemComponent->GetActiveGameplayEffect(Handle)->GetTimeRemaining(GetWorld()->GetTimeSeconds());
		Duration.bIsInfinite = Duration.Duration < 0.f;
		// Apply configured effects
		StatusEffectRow.ApplyEffect(GetOwner(), Duration);
		// Broadcast to other consumers that may need to know
		OnStatusEffectAddedDelegate.Broadcast(
			FOnStatusEffectAddedPayload(
				GetOwner(),
				UElectricCastleAbilitySystemLibrary::GetAbilitySystemComponent(GetOwner()),
				StatusEffectTags,
				Duration
			)
		);
		if (bDebug)
		{
			UE_LOG(LogTemp, Warning, TEXT("[%s] Status Effect Added: %s"), *GetName(), *StatusEffectRow.EffectTag.ToString());
		}
	}
}

void UStatusEffectManagerComponent::OnEffectRemoved(const FActiveGameplayEffect& ActiveGameplayEffect) const
{
	const UElectricCastleGameDataSubsystem* GameDataSubsystem = UElectricCastleGameDataSubsystem::Get(GetOwner());
	const UStatusEffectConfig* StatusEffectConfig = GameDataSubsystem ? GameDataSubsystem->GetStatusEffectConfig() : nullptr;
	if (!StatusEffectConfig)
	{
		return;
	}
	FGameplayTagContainer SpecTags;
	ActiveGameplayEffect.Spec.GetAllGrantedTags(SpecTags);
	// Extract tags
	const FGameplayTagContainer StatusEffectTags = StatusEffectConfig->GetMatchingStatusEffectTags(SpecTags);
	if (const FStatusEffectRow& StatusEffectRow = StatusEffectConfig->FindStatusEffectRowByTags(SpecTags); StatusEffectRow.IsValid())
	{
		// Remove configured effects
		StatusEffectRow.RemoveEffect(GetOwner());
		// Broadcast to other consumers that may need to know
		OnStatusEffectRemovedDelegate.Broadcast(
			FOnStatusEffectRemovedPayload(
				GetOwner(),
				UElectricCastleAbilitySystemLibrary::GetAbilitySystemComponent(GetOwner()),
				StatusEffectTags
			)
		);
		if (bDebug)
		{
			UE_LOG(LogTemp, Warning, TEXT("[%s] Status Effect Removed: %s"), *GetName(), *StatusEffectTags.ToString());
		}
	}
}

void UStatusEffectManagerComponent::HandleOwnerDeath(AActor* DeadActor)
{
	for (const auto& [StatusEffectTag, FXInstance] : StatusEffectFXInstances)
	{
		if (IsValid(FXInstance))
		{
			FXInstance->DestroyComponent();
		}
	}
	StatusEffectFXInstances.Empty();
	if (IsValid(StatusEffectBarWidget))
	{
		StatusEffectBarWidget->SetVisibility(ESlateVisibility::Hidden);
	}
	if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(DeadActor))
	{
		AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.RemoveAll(this);
		AbilitySystemComponent->OnAnyGameplayEffectRemovedDelegate().RemoveAll(this);
	}
}
