// Copyright Alien Shores


#include "AbilitySystem/Ability/Offensive/BeamGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"
#include "Actor/Beam/BeamActor.h"
#include "Actor/Beam/DamageBeamActor.h"
#include "Actor/Mesh/SocketManagerActor.h"
#include "Actor/Mesh/SocketManagerComponent.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"

void UBeamGameplayAbility::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	if (GetWorld()->GetTimerManager().IsTimerActive(DelayTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(DelayTimerHandle);
	}
	if (IsValid(BeamActor))
	{
		BeamActor->Terminate();
	}
	CommitAbilityCooldown(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false);
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}


void UBeamGameplayAbility::PlayAbilityMontage_Implementation()
{
	// No native implementation
}

void UBeamGameplayAbility::PlayAbilitySoundCue_Implementation()
{
	// No native implementation
}

int32 UBeamGameplayAbility::GetCascadeTargetsCount_Implementation() const
{
	return MaxCascades.GetValueAtLevel(GetAbilityLevel());
}

void UBeamGameplayAbility::SpawnBeam_Implementation()
{
	if (!HasAuthority(&CurrentActivationInfo))
	{
		// only spawn beams on the server
		return;
	}
	// determine spawn location
	AActor* SourceObject = nullptr;
	if (bIsWeaponAbility)
	{
		SourceObject = ICombatInterface::GetWeapon(GetAvatarActorFromActorInfo());
	}
	if (!SourceObject)
	{
		SourceObject = GetAvatarActorFromActorInfo();
	}
	BeamActor = GetWorld()->SpawnActorDeferred<ABeamActor>(
		BeamClass,
		SourceObject->GetActorTransform(),
		SourceObject,
		Cast<APawn>(GetAvatarActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (!BeamActor)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Failed to spawn beam actor. Check ability config to ensure beam type is set!"), *GetName());
		EndAbility(GetCurrentAbilitySpec()->Handle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	if (bOverrideBeamProperties)
	{
		switch (BeamCascadeType)
		{
		case EBeamCascadeType::None:
			BeamActor->SetNoCascadeParams();
			break;
		case EBeamCascadeType::Linear:
			BeamActor->SetLinearCascadeParams(FBeamCascadeLinearParams(CascadeDistance.GetValueAtLevel(GetAbilityLevel()), MaxCascades.GetValueAtLevel(GetAbilityLevel())));
			break;
		default:
			UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Unhandled cascade type: %s"), *GetName(), *UEnum::GetValueAsString(BeamCascadeType))
			break;
		}
		BeamActor->SetTraceParams(MaxBeamLength.GetValueAtLevel(GetAbilityLevel()), BeamRadius.GetValueAtLevel(GetAbilityLevel()), BeamTraceChannel, bDebug);
		BeamActor->SetTraceOrigin(GetAvatarActorFromActorInfo());
		if (ADamageBeamActor* DamageBeamActor = Cast<ADamageBeamActor>(BeamActor))
		{
			DamageBeamActor->SetDamageEffectParams(MakeDamageEffectParamsFromClassDefaults(nullptr));
		}
	}
	BeamActor->FinishSpawning(SourceObject->GetActorTransform());
	if (const USocketManagerComponent* SocketManagerComponent = ISocketManagerActor::GetSocketManagerComponent(SourceObject))
	{
		SocketManagerComponent->AttachByTag(BeamActor->GetRootComponent(), SocketTag);
	}
	else
	{
		BeamActor->AttachToComponent(SourceObject->GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
	}
}

void UBeamGameplayAbility::OnDelayedRelease()
{
	EndAbility(GetCurrentAbilitySpec()->Handle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UBeamGameplayAbility::OnInputRelease_Implementation(const float TimeHeld)
{
	if (TimeHeld < MinimumSpellTime)
	{
		// Enforce minimum spell time
		GetWorld()->GetTimerManager().SetTimer(
			DelayTimerHandle,
			this,
			&UBeamGameplayAbility::OnDelayedRelease,
			MinimumSpellTime - TimeHeld,
			false
		);
	}
	else
	{
		EndAbility(GetCurrentAbilitySpec()->Handle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}
