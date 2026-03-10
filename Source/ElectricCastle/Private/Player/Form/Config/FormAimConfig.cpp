// Copyright Alien Shores


#include "Player/Form/Config/FormAimConfig.h"

#include "AbilitySystemBlueprintLibrary.h"

#include "AbilitySystem/ElectricCastleAbilitySystemComponent.h"
#include "AbilitySystem/ElectricCastleAbilitySystemInterface.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"

#include "Player/Aim/AimActorInterface.h"
#include "Player/Aim/AimController.h"

#include "Tags/ElectricCastleGameplayTags.h"

UFormAimConfig::UFormAimConfig()
{
	AimAbilityTag = FElectricCastleGameplayTags::Get().Abilities_Other_Aim;
}

void UFormAimConfig::OnFormActivated_Implementation(AActor* Actor, UPlayerFormChangeComponent* FormChangeComponent)
{
	Super::OnFormActivated_Implementation(Actor, FormChangeComponent);
	if (UAimController* AimController = IAimActorInterface::GetAimController(Actor))
	{
		const int32 Level = IElectricCastleAbilitySystemInterface::GetCharacterLevel(Actor);
		AimController->SetTraceParams(TraceDistance.GetValueAtLevel(Level), TraceRadius.GetValueAtLevel(Level));
		AimController->SetHideCrosshair(!bShouldShowCrosshair);
		AimController->SetCanAim(true);
	}
	if (AimAbilityTag.IsValid())
	{
		if (UElectricCastleAbilitySystemComponent* AbilitySystemComponent = UElectricCastleAbilitySystemLibrary::GetAbilitySystemComponent(Actor))
		{
			AbilitySystemComponent->GrantAbilitiesWithTag(AimAbilityTag);
		}
	}
}

void UFormAimConfig::OnFormDeactivated_Implementation(AActor* Actor, UPlayerFormChangeComponent* FormChangeComponent)
{
	Super::OnFormDeactivated_Implementation(Actor, FormChangeComponent);
	if (UAimController* AimController = IAimActorInterface::GetAimController(Actor))
	{
		AimController->SetTraceParams(0.f, 0.f);
		AimController->SetHideCrosshair(true);
		AimController->SetCanAim(false);
	}
	if (AimAbilityTag.IsValid())
	{
		if (UElectricCastleAbilitySystemComponent* AbilitySystemComponent = UElectricCastleAbilitySystemLibrary::GetAbilitySystemComponent(Actor))
		{
			AbilitySystemComponent->RemoveAbilitiesWithTag(AimAbilityTag);
		}
	}
}
