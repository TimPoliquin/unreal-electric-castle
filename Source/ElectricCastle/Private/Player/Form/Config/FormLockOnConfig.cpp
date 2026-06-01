// Copyright Alien Shores


#include "Player/Form/Config/FormLockOnConfig.h"

#include "AbilitySystem/ElectricCastleAbilitySystemComponent.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "Player/LockOn/LockOnActor.h"
#include "Player/LockOn/LockOnController.h"
#include "Tags/ElectricCastleGameplayTags.h"

UFormLockOnConfig::UFormLockOnConfig()
{
	LockOnAbilityTag = FElectricCastleGameplayTags::Get().Abilities_Other_LockOn;
}

void UFormLockOnConfig::OnFormActivated_Implementation(AActor* Actor, UPlayerFormChangeComponent* FormChangeComponent)
{
	if (ULockOnController* LockOnController = ILockOnActor::GetLockOnController(Actor))
	{
		LockOnController->SetLockOnSupported(true);
	}
	if (LockOnAbilityTag.IsValid() && bAutoGrantAndRemove)
	{
		if (UElectricCastleAbilitySystemComponent* AbilitySystemComponent = UElectricCastleAbilitySystemLibrary::GetAbilitySystemComponent(Actor))
		{
			AbilitySystemComponent->GrantAbilitiesWithTag(LockOnAbilityTag);
		}
	}
}

void UFormLockOnConfig::OnFormDeactivated_Implementation(AActor* Actor, UPlayerFormChangeComponent* FormChangeComponent)
{
	Super::OnFormDeactivated_Implementation(Actor, FormChangeComponent);
	if (ULockOnController* LockOnController = ILockOnActor::GetLockOnController(Actor))
	{
		LockOnController->SetLockOnSupported(false);
	}
	if (LockOnAbilityTag.IsValid() && bAutoGrantAndRemove)
	{
		if (UElectricCastleAbilitySystemComponent* AbilitySystemComponent = UElectricCastleAbilitySystemLibrary::GetAbilitySystemComponent(Actor))
		{
			AbilitySystemComponent->RemoveAbilitiesWithTag(LockOnAbilityTag);
		}
	}
}
