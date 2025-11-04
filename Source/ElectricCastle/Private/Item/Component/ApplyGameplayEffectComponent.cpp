// Copyright Alien Shores


#include "Item/Component/ApplyGameplayEffectComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "ElectricCastle/ElectricCastle.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Kismet/GameplayStatics.h"
#include "Utils/TagUtils.h"


// Sets default values for this component's properties
UApplyGameplayEffectComponent::UApplyGameplayEffectComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	ApplyToTags.Add(TAG_PLAYER);
	ApplyToTags.Add(TAG_ENEMY);
}


bool UApplyGameplayEffectComponent::CheckPreRequisites(AActor* PickupActor) const
{
	return TagUtils::HasAnyTag(PickupActor, ApplyToTags);
}

void UApplyGameplayEffectComponent::OnOverlap(AActor* TargetActor)
{
	if (GameplayEffectConfigs.IsEmpty())
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s][%s] No overlap effect configured for effect component!"), *GetOwner()->GetName(), *GetName());
		return;
	}
	if (!CheckPreRequisites(TargetActor))
	{
		return;
	}
	for (auto GameplayEffectConfig : GameplayEffectConfigs)
	{
		if (!GameplayEffectConfig.IsValid())
		{
			UE_LOG(LogElectricCastle, Warning, TEXT("[%s][%s] Invalid overlap effect configured for component"), *GetOwner()->GetName(), *GetName())
		}
		if (GameplayEffectConfig.IsApplyOnOverlap())
		{
			ApplyEffectToTarget(TargetActor, GameplayEffectConfig);
		}
	}
	OnPlayerOverlapStart.Broadcast(TargetActor);
}

void UApplyGameplayEffectComponent::OnEndOverlap(AActor* TargetActor)
{
	if (!TagUtils::HasAnyTag(TargetActor, ApplyToTags))
	{
		// effect does not apply to target.
		return;
	}
	for (auto GameplayEffectConfig : GameplayEffectConfigs)
	{
		if (GameplayEffectConfig.IsApplyOnEndOverlap())
		{
			ApplyEffectToTarget(TargetActor, GameplayEffectConfig);
		}
		if (GameplayEffectConfig.IsRemoveOnEndOverlap())
		{
			RemoveEffectsFromTarget(TargetActor);
		}
	}
	OnPlayerOverlapEnd.Broadcast(TargetActor);
}

void UApplyGameplayEffectComponent::SetDefaults(
	const TArray<FGameplayEffectConfig>& InEffectConfig,
	const bool InDestroyOnEffectApplication,
	const TArray<FName>& InApplyToTags
)
{
	GameplayEffectConfigs = InEffectConfig;
	bDestroyOnEffectApplication = InDestroyOnEffectApplication;
	ApplyToTags = InApplyToTags;
}

bool UApplyGameplayEffectComponent::IsAffectingActor(AActor* Actor) const
{
	if (const UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor))
	{
		for (const TPair HandlePair : ActiveEffectHandles)
		{
			if (HandlePair.Value == AbilitySystemComponent)
			{
				return true;
			}
		}
	}
	return false;
}

void UApplyGameplayEffectComponent::ApplyEffectToTarget(AActor* TargetActor, const FGameplayEffectConfig& GameplayEffectConfig)
{
	if (!GameplayEffectConfig.GameplayEffectClass)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] No effect configured!"), *GetOwner()->GetName(), *GetName());
		return;
	}
	if (UAbilitySystemComponent* TargetAbilitySystem = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
		TargetActor
	))
	{
		FGameplayEffectContextHandle EffectContextHandle = TargetAbilitySystem->MakeEffectContext();
		EffectContextHandle.AddSourceObject(this);
		const FGameplayEffectSpecHandle EffectSpecHandle = TargetAbilitySystem->MakeOutgoingSpec(
			GameplayEffectConfig.GameplayEffectClass,
			ActorLevel,
			EffectContextHandle
		);
		const FActiveGameplayEffectHandle ActiveEffectHandle = TargetAbilitySystem->ApplyGameplayEffectSpecToSelf(
			*EffectSpecHandle.Data.Get()
		);
		const bool bIsInfiniteEffect = UElectricCastleAbilitySystemLibrary::IsInfiniteEffect(EffectSpecHandle);
		if (bIsInfiniteEffect && GameplayEffectConfig.IsRemoveOnEndOverlap())
		{
			ActiveEffectHandles.Add(ActiveEffectHandle, TargetAbilitySystem);
		}
		if (bDestroyOnEffectApplication && !bIsInfiniteEffect)
		{
			GetWorld()->DestroyActor(GetOwner());
		}
	}
}

void UApplyGameplayEffectComponent::RemoveEffectsFromTarget(AActor* TargetActor)
{
	if (UAbilitySystemComponent* TargetAbilitySystem = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
		TargetActor
	))
	{
		TArray<FActiveGameplayEffectHandle> HandlesToRemove;
		for (auto HandlePair : ActiveEffectHandles)
		{
			if (TargetAbilitySystem == HandlePair.Value)
			{
				TargetAbilitySystem->RemoveActiveGameplayEffect(HandlePair.Key, 1);
				HandlesToRemove.Add(HandlePair.Key);
			}
		}
		for (auto Handle : HandlesToRemove)
		{
			ActiveEffectHandles.FindAndRemoveChecked(Handle);
		}
	}
}
