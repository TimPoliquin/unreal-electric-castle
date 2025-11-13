// Copyright Alien Shores


#include "UI/ViewModel/MVVM_PlayerAbilityStates.h"

#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Tags/ElectricCastleGameplayTags.h"
#include "UI/ViewModel/MVVM_AbilityState.h"

void UMVVM_PlayerAbilityStates::InitializeDependencies(APlayerState* PlayerState)
{
	if (!AbilityStateViewModelClass)
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[%s] AbilityStateViewModelClass is null"), *GetName());
	}
	const FElectricCastleGameplayTags& GameplayTags = FElectricCastleGameplayTags::Get();
	Ability_01 = NewObject<UMVVM_AbilityState>(this, AbilityStateViewModelClass, FName("Ability_01"));
	Ability_01->SetInputTag(GameplayTags.InputTag_Action1);
	Ability_01->InitializeDependencies(PlayerState);
	Ability_02 = NewObject<UMVVM_AbilityState>(this, AbilityStateViewModelClass, FName("Ability_02"));
	Ability_02->SetInputTag(GameplayTags.InputTag_Action2);
	Ability_02->InitializeDependencies(PlayerState);
	Ability_03 = NewObject<UMVVM_AbilityState>(this, AbilityStateViewModelClass, FName("Ability_03"));
	Ability_03->SetInputTag(GameplayTags.InputTag_Action3);
	Ability_03->InitializeDependencies(PlayerState);
	Ability_04 = NewObject<UMVVM_AbilityState>(this, AbilityStateViewModelClass, FName("Ability_04"));
	Ability_04->SetInputTag(GameplayTags.InputTag_Action4);
	Ability_04->InitializeDependencies(PlayerState);
}
