// Copyright Alien Shores


#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"


UDebuffNiagaraComponent::UDebuffNiagaraComponent()
{
	bAutoActivate = false;
}


void UDebuffNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner());
	if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
		GetOwner()
	))
	{
		AbilitySystemComponent->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(
			this,
			&UDebuffNiagaraComponent::OnDebuffTagChanged
		);
	}
	else if (CombatInterface)
	{
		CombatInterface->GetOnAbilitySystemRegisteredDelegate().AddWeakLambda(
			this,
			[this](UAbilitySystemComponent* InAbilitySystemComponent)
			{
				InAbilitySystemComponent->RegisterGameplayTagEvent(
					DebuffTag,
					EGameplayTagEventType::NewOrRemoved
				).AddUObject(this, &UDebuffNiagaraComponent::OnDebuffTagChanged);
			}
		);
	}
	if (CombatInterface)
	{
		CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UDebuffNiagaraComponent::OnOwnerDeath);
	}
}

void UDebuffNiagaraComponent::OnDebuffTagChanged(FGameplayTag GameplayTag, const int StackCount)
{
	if (!IsValid(GetOwner()))
	{
		return;
	}
	if (ICombatInterface::IsDead(GetOwner()))
	{
		return;
	}
	if (StackCount > 0)
	{
		Activate(false);
	}
	else
	{
		Deactivate();
	}
}

void UDebuffNiagaraComponent::OnOwnerDeath(AActor* Actor)
{
	Deactivate();
}
