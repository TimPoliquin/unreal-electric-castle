// Copyright Alien Shores


#include "AbilitySystem/Effect/AnimNotifyState_GameplayEffect.h"

#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"

void UAnimNotifyState_GameplayEffect::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	for (auto& [GameplayEffect, Level, bUseMagnitude, MagnitudeTag, Magnitude] : GameplayEffects)
	{
		if (bUseMagnitude)
		{
			Handles.Add(UElectricCastleAbilitySystemLibrary::ApplyBasicGameplayEffectWithMagnitude(MeshComp->GetOwner(), GameplayEffect, Level, MagnitudeTag, Magnitude));
		}
		else
		{
			Handles.Add(UElectricCastleAbilitySystemLibrary::ApplyBasicGameplayEffect(MeshComp->GetOwner(), GameplayEffect, Level));
		}
	}
}

void UAnimNotifyState_GameplayEffect::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	for (FActiveGameplayEffectHandle& Handle : Handles)
	{
		UElectricCastleAbilitySystemLibrary::RemoveGameplayEffect(MeshComp->GetOwner(), Handle);
	}
}
