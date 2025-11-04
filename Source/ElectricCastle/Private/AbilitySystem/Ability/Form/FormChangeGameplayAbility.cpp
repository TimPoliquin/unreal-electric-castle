// Copyright Alien Shores


#include "AbilitySystem/Ability/Form/FormChangeGameplayAbility.h"

#include "Player/Form/FormChangeActorInterface.h"
#include "Player/Form/PlayerFormChangeComponent.h"
#include "Tags/ElectricCastleGameplayTags.h"

UFormChangeGameplayAbility::UFormChangeGameplayAbility()
{
	ActivationBlockedTags.AddTag(FElectricCastleGameplayTags::Get().Cooldown_Form);
	ActivationBlockedTags.AddTag(FElectricCastleGameplayTags::Get().Player_Block_ChangeForm);
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

bool UFormChangeGameplayAbility::CanChangeForms_Implementation(const FGameplayTag NewFormTag) const
{
	if (const UPlayerFormChangeComponent* FormChangeComponent = IFormChangeActorInterface::GetFormChangeComponent(GetAvatarActorFromActorInfo()))
	{
		if (!FormChangeComponent->GetCurrentFormTag().MatchesTagExact(NewFormTag))
		{
			return true;
		}
	}
	return false;
}

void UFormChangeGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                 const FGameplayEventData* TriggerEventData)
{
	if (!CanChangeForms(FormTag))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	UPlayerFormChangeComponent* FormChangeComponent = IFormChangeActorInterface::GetFormChangeComponent(GetAvatarActorFromActorInfo());
	FormChangeComponent->ChangeForm(FormTag);
	CommitAbility(Handle, ActorInfo, ActivationInfo);
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
