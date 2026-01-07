// Copyright Alien Shores


#include "Character/Status/StatusEffectFXComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/ElectricCastleAbilitySystemInterface.h"
#include "GameFramework/Character.h"


UStatusEffectFXComponent::UStatusEffectFXComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UStatusEffectFXComponent::BeginPlay()
{
	Super::BeginPlay();
	if (IElectricCastleAbilitySystemInterface::IsAbilitySystemReady(GetOwner()))
	{
		AddStatusEffectTagListeners(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()));
	}
	else if (IElectricCastleAbilitySystemInterface* AbilitySystemInterface = Cast<IElectricCastleAbilitySystemInterface>(GetOwner()))
	{
		AbilitySystemInterface->GetOnAbilitySystemRegisteredDelegate().AddUObject(this, &UStatusEffectFXComponent::AddStatusEffectTagListeners);
	}
}

void UStatusEffectFXComponent::AddStatusEffectTagListeners(UAbilitySystemComponent* AbilitySystemComponent)
{
	for (const auto& Pair : StatusEffectFX)
	{
		AbilitySystemComponent->RegisterGameplayTagEvent(Pair.Key, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UStatusEffectFXComponent::OnStatusEffectTagCountChanged);
	}
}

bool UStatusEffectFXComponent::HasFXInstanceByTag(const FGameplayTag& StatusEffectTag) const
{
	return StatusEffectFXInstances.ContainsByPredicate([StatusEffectTag](const FStatusEffectFXInstance& Instance)
	{
		return Instance.StatusEffectTag.MatchesTagExact(StatusEffectTag);
	});
}

FStatusEffectFXInstance UStatusEffectFXComponent::FindFXInstanceByTag(const FGameplayTag& StatusEffectTag) const
{
	const FStatusEffectFXInstance* FXInstance = StatusEffectFXInstances.FindByPredicate([StatusEffectTag](const FStatusEffectFXInstance& Instance)
	{
		return Instance.StatusEffectTag.MatchesTagExact(StatusEffectTag);
	});
	if (FXInstance)
	{
		return *FXInstance;
	}
	return FStatusEffectFXInstance();
}

void UStatusEffectFXComponent::SpawnStatusEffectFX_Implementation(const FGameplayTag& InStatusEffectTag)
{
	if (HasFXInstanceByTag(InStatusEffectTag))
	{
		return;
	}
	if (const ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		StatusEffectFXInstances.Add(FStatusEffectFXInstance(
			InStatusEffectTag,
			UNiagaraFunctionLibrary::SpawnSystemAttached(
				StatusEffectFX[InStatusEffectTag],
				// DEVNOTE: Attaching to the character mesh is perhaps an incomplete solution, as it does not include clothing or the head.
				Character->GetMesh(),
				NAME_None,
				FVector::ZeroVector,
				FRotator::ZeroRotator,
				EAttachLocation::Type::SnapToTarget,
				true,
				true
			)));
	}
}

void UStatusEffectFXComponent::DestroyStatusEffectFX_Implementation(const FGameplayTag& InStatusEffectTag)
{
	if (const FStatusEffectFXInstance& StatusEffectFXInstance = FindFXInstanceByTag(InStatusEffectTag); StatusEffectFXInstance.IsValid())
	{
		if (IsValid(StatusEffectFXInstance.FXInstance))
		{
			StatusEffectFXInstance.FXInstance->DestroyComponent();
		}
		StatusEffectFXInstances.RemoveAll([InStatusEffectTag](const FStatusEffectFXInstance& Instance)
		{
			return Instance.StatusEffectTag.MatchesTagExact(InStatusEffectTag);
		});
	}
}

void UStatusEffectFXComponent::OnStatusEffectTagCountChanged(FGameplayTag InStatusEffectTag, int32 InCount)
{
	if (InCount > 0)
	{
		SpawnStatusEffectFX(InStatusEffectTag);
	}
	else if (InCount <= 0)
	{
		DestroyStatusEffectFX(InStatusEffectTag);
	}
}
