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
	} else if (IElectricCastleAbilitySystemInterface* AbilitySystemInterface = Cast<IElectricCastleAbilitySystemInterface>(GetOwner()))
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

void UStatusEffectFXComponent::SpawnStatusEffectFX(const FGameplayTag& InStatusEffectTag)
{
	if (const ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		StatusEffectFXInstances.Add(InStatusEffectTag, UNiagaraFunctionLibrary::SpawnSystemAttached(
			StatusEffectFX[InStatusEffectTag], 
			// DEVNOTE: Attaching to the character mesh is perhaps an incomplete solution, as it does not include clothing or the head.
			Character->GetMesh(),  
			NAME_None, 
			FVector::ZeroVector, 
			FRotator::ZeroRotator, 
			EAttachLocation::Type::SnapToTarget, 
			true,
			true
		));
	}
}

void UStatusEffectFXComponent::DestroyStatusEffectFX(const FGameplayTag& InStatusEffectTag)
{
	if (StatusEffectFXInstances.Contains(InStatusEffectTag))
	{
		StatusEffectFXInstances[InStatusEffectTag]->DestroyComponent();
		StatusEffectFXInstances.Remove(InStatusEffectTag);
	}
}

void UStatusEffectFXComponent::OnStatusEffectTagCountChanged(FGameplayTag InStatusEffectTag, int32 InCount)
{
	if (InCount > 0 && !StatusEffectFXInstances.Contains(InStatusEffectTag))
	{
		SpawnStatusEffectFX(InStatusEffectTag);
	} else if (InCount <= 0 && StatusEffectFXInstances.Contains(InStatusEffectTag))
	{
		DestroyStatusEffectFX(InStatusEffectTag);
	}
}


