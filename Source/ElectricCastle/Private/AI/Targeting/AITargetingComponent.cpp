// Copyright Alien Shores


#include "AI/Targeting/AITargetingComponent.h"

#include "AbilitySystemComponent.h"
#include "AI/Targeting/AITargetProviderInterface.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "AbilitySystem/ElectricCastleAttributeSet.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Interaction/CombatInterface.h"

UAITargetingComponent::UAITargetingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UAITargetingComponent::BeginPlay()
{
	Super::BeginPlay();
}

AActor* UAITargetingComponent::GetCurrentTarget() const
{
	return Target.IsValid() ? Target.Get() : nullptr;
}

void UAITargetingComponent::SetCurrentTarget(AActor* InTarget)
{
	AActor* OldTarget = GetCurrentTarget();
	AActor* NewTarget = ToEffectiveTarget(InTarget);
	if (OldTarget == NewTarget)
	{
		// ignore when the same target is set
		return;
	}
	RemoveTargetEventListeners(OldTarget);
	if (!IsValid(NewTarget))
	{
		// target is not valid - just clear the value
		Target = nullptr;
	}
	else
	{
		// Target is a viable target - set it and setup listeners
		Target = NewTarget;
		SetupTargetEventListeners(NewTarget);
	}
	OnTargetChanged.Broadcast(FAITargetChangedPayload(GetOwner(), this, OldTarget, NewTarget));
}


bool UAITargetingComponent::HasCurrentTarget() const
{
	return Target.IsValid();
}

void UAITargetingComponent::SetTargetProvider(AActor* InTargetProvider)
{
	if (IsValid(InTargetProvider) && InTargetProvider->Implements<UAITargetProviderInterface>())
	{
		TargetProvider = InTargetProvider;
	}
	else
	{
		UE_LOG(
			LogElectricCastle,
			Warning,
			TEXT("[%s:%s] Attempted to set invalid target provider: %s"),
			*GetOwner()->GetName(),
			*GetName(),
			InTargetProvider ? *InTargetProvider->GetName() : *FString("nullptr")
		)
	}
}

void UAITargetingComponent::SetupTargetEventListeners(AActor* InTarget)
{
	if (!IsValid(InTarget))
	{
		return;
	}
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(InTarget))
	{
		CombatInterface->GetOnDeathDelegate().AddUniqueDynamic(this, &UAITargetingComponent::HandleTargetDeath);
	}
	if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InTarget))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UElectricCastleAttributeSet::GetVisibilityAttribute())
		                      .AddUObject(this, &UAITargetingComponent::HandleTargetVisibilityChanged);
	}
}

void UAITargetingComponent::RemoveTargetEventListeners(AActor* InTarget)
{
	if (!IsValid(InTarget))
	{
		return;
	}
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(InTarget))
	{
		CombatInterface->GetOnDeathDelegate().RemoveAll(this);
	}
	if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InTarget))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UElectricCastleAttributeSet::GetVisibilityAttribute()).RemoveAll(this);
	}
}

void UAITargetingComponent::HandleTargetVisibilityChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	if (OnAttributeChangeData.NewValue <= 0.f)
	{
		SetCurrentTarget(nullptr);
	}
}

void UAITargetingComponent::HandleTargetDeath(AActor* DeadActor)
{
	if (Target.IsValid() && Target.Get() == DeadActor)
	{
		SetCurrentTarget(nullptr);
	}
}

AActor* UAITargetingComponent::ToEffectiveTarget(AActor* InTarget) const
{
	if (!IsValid(InTarget))
	{
		// do not allow invalid objects 
		return nullptr;
	}
	if (!ICombatInterface::IsAlive(InTarget))
	{
		// do not allow targets that are not alive
		return nullptr;
	}
	if (UElectricCastleAbilitySystemLibrary::GetVisibilityAttributeValue(InTarget) <= 0.f)
	{
		// do not allow invisible targets
		return nullptr;
	}
	return InTarget;
}
