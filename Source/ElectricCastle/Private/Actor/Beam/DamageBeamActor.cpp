// Copyright Alien Shores


#include "Actor/Beam/DamageBeamActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
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

FActiveGameplayEffectHandle ADamageBeamActor::ApplyBeamTargetEffect_Implementation(AActor* Target)
{
	if (UAbilitySystemComponent* TargetAbilitySystem = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target))
	{
		FDamageEffectParams BeamEffectParams = DamageEffectParams;
		BeamEffectParams.TargetAbilitySystemComponent = TargetAbilitySystem;
		return UElectricCastleAbilitySystemLibrary::ApplyDamageEffect(BeamEffectParams);
	}
	return FActiveGameplayEffectHandle();
}

void ADamageBeamActor::InitializeChildBeamProperties_Implementation(ABeamActor* ChildBeam, const FHitResult& HitResult)
{
	Super::InitializeChildBeamProperties_Implementation(ChildBeam, HitResult);
	if (ADamageBeamActor* DamageBeam = Cast<ADamageBeamActor>(ChildBeam))
	{
		DamageBeam->SetDamageEffectParams(DamageEffectParams);
	}
}
