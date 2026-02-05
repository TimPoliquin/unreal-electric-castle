// Copyright Alien Shores


#include "Actor/Beam/DamageBeamActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "Actor/BeamTargetInterface.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ADamageBeamActor::ADamageBeamActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ADamageBeamActor::SetDamageEffectParams(const FDamageEffectParams& InParams)
{
	DamageEffectParams = InParams;
}

void ADamageBeamActor::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADamageBeamActor, DamageEffectParams)
}

void ADamageBeamActor::Terminate_Implementation()
{
	for (TTuple<TObjectPtr<AActor>, FTimerHandle>& HandlePair : DamageEffectTimerHandles)
	{
		GetWorldTimerManager().ClearTimer(HandlePair.Value);
	}
	DamageEffectTimerHandles.Empty();
	Super::Terminate_Implementation();
}

FActiveGameplayEffectHandle ADamageBeamActor::ApplyBeamTargetEffect_Implementation(AActor* Target)
{
	if (!HasAuthority())
	{
		return FActiveGameplayEffectHandle();
	}
	if (BeamTypeTag.IsValid() && IBeamTargetInterface::IsBeamTarget(Target))
	{
		IBeamTargetInterface::Execute_HandleBeamApplied(Target, BeamTypeTag);
	}
	if (DealDamagePeriodically)
	{
		FTimerHandle& TimerHandle = DamageEffectTimerHandles.Add(Target);

		GetWorldTimerManager().SetTimer(TimerHandle, [this, Target]()
		{
			if (ICombatInterface::IsAlive(Target))
			{
				ApplyDamageEffect(Target);
			}
			else if (!IBeamTargetInterface::IsBeamTarget(Target))
			{
				FActiveGameplayEffectHandle PlaceholderHandle;
				RemoveBeamTargetEffect(Target, PlaceholderHandle);
			}
		}, DamagePeriod, true);
	}
	else
	{
		ApplyDamageEffect(Target);
	}
	return FActiveGameplayEffectHandle();
}

void ADamageBeamActor::RemoveBeamTargetEffect_Implementation(AActor* Target, FActiveGameplayEffectHandle& Handle)
{
	Super::RemoveBeamTargetEffect_Implementation(Target, Handle);

	if (FTimerHandle* TimerHandle = DamageEffectTimerHandles.Find(Target))
	{
		GetWorldTimerManager().ClearTimer(*TimerHandle);
		DamageEffectTimerHandles.Remove(Target);
	}
}

void ADamageBeamActor::InitializeChildBeamProperties_Implementation(ABeamActor* ChildBeam, const FHitResult& HitResult)
{
	Super::InitializeChildBeamProperties_Implementation(ChildBeam, HitResult);
	if (ADamageBeamActor* DamageBeam = Cast<ADamageBeamActor>(ChildBeam))
	{
		DamageBeam->SetDamageEffectParams(DamageEffectParams);
	}
}

void ADamageBeamActor::ApplyDamageEffect(AActor* Target) const
{
	if (!HasAuthority())
	{
		return;
	}
	if (UAbilitySystemComponent* TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target))
	{
		FDamageEffectParams BeamEffectParams = DamageEffectParams;
		BeamEffectParams.TargetAbilitySystemComponent = TargetAbilitySystemComponent;
		UElectricCastleAbilitySystemLibrary::ApplyDamageEffect(BeamEffectParams);
	}
}
