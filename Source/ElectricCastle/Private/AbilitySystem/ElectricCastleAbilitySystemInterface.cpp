// Copyright Alien Shores


#include "AbilitySystem/ElectricCastleAbilitySystemInterface.h"

#include "AbilitySystem/ElectricCastleAbilitySystemComponent.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"


// Add default functionality here for any IElectricCastleAbilitySystemInterface functions that are not pure virtual.
int32 IElectricCastleAbilitySystemInterface::GetCharacterLevel(const UObject* Character)
{
	if (IsValid(Character) && Character->Implements<UElectricCastleAbilitySystemInterface>())
	{
		return Execute_GetCharacterLevel(Character);
	}
	return -1;
}

bool IElectricCastleAbilitySystemInterface::IsAbilitySystemReady(AActor* Actor)
{
	if (const UElectricCastleAbilitySystemComponent* AbilitySystemComponent = UElectricCastleAbilitySystemLibrary::GetAbilitySystemComponent(Actor))
	{
		return AbilitySystemComponent->HasFiredOnAbilitiesGivenDelegate();
	}
	return false;
}
