// Copyright Alien Shores


#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/ElectricCastleAbilitySystemComponent.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "Interaction/CombatInterface.h"


// Sets default values for this component's properties
UPassiveNiagaraComponent::UPassiveNiagaraComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bAutoActivate = false;
	// ...
}


// Called when the game starts
void UPassiveNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();
	if (UElectricCastleAbilitySystemComponent* AbilitySystemComponent = Cast<UElectricCastleAbilitySystemComponent>(
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())
	))
	{
		AbilitySystemComponent->OnActivatePassiveEffectDelegate.AddUObject(
			this,
			&UPassiveNiagaraComponent::OnPassiveActivate
		);
		ActivateIfAbilityIsAlreadyActive(AbilitySystemComponent);
	}
	else if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner()))
	{
		CombatInterface->GetOnAbilitySystemRegisteredDelegate().AddLambda(
			[this](UAbilitySystemComponent* AbilitySystemComponent)
			{
				if (UElectricCastleAbilitySystemComponent* AwaitedComponent = Cast<UElectricCastleAbilitySystemComponent>(
					AbilitySystemComponent
				))
				{
					AwaitedComponent->OnActivatePassiveEffectDelegate.AddUObject(
						this,
						&UPassiveNiagaraComponent::OnPassiveActivate
					);
					ActivateIfAbilityIsAlreadyActive(AwaitedComponent);
				}
			}
		);
	}
}

void UPassiveNiagaraComponent::OnPassiveActivate(const FGameplayTag& AbilityTag, const bool bActivate)
{
	if (AbilityTag.MatchesTag(PassiveSpellTag))
	{
		if (bActivate && !IsActive())
		{
			Activate();
		}
		else if (!bActivate)
		{
			Deactivate();
		}
	}
}

void UPassiveNiagaraComponent::ActivateIfAbilityIsAlreadyActive(UElectricCastleAbilitySystemComponent* AbilitySystemComponent)
{
	if (AbilitySystemComponent->HasFiredOnAbilitiesGivenDelegate())
	{
		if (UElectricCastleAbilitySystemLibrary::IsAbilityEquipped(AbilitySystemComponent, PassiveSpellTag))
		{
			Activate();
		}
	}
}
