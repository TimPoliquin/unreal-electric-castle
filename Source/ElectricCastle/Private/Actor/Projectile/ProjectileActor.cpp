// Copyright Alien Shores


#include "Actor/Projectile/ProjectileActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "Actor/ElectricCastleActorBlueprintFunctionLibrary.h"
#include "ElectricCastle/ElectricCastle.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Components/AudioComponent.h"
#include "Interaction/CombatInterface.h"
#include "Net/UnrealNetwork.h"
#include "Utils/TagUtils.h"

AProjectileActor::AProjectileActor()
{
	PrimaryActorTick.bCanEverTick = false;
	// DEVNOTE: Ensures that the state is replicated from server -> client
	bReplicates = true;
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	SetRootComponent(Sphere);
	Sphere->SetCollisionObjectType(ECC_Projectile);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 550.f;
	ProjectileMovement->MaxSpeed = 550.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
}

void AProjectileActor::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AProjectileActor, DamageEffectParams);
}

UProjectileMovementComponent* AProjectileActor::GetProjectileMovementComponent() const
{
	return ProjectileMovement;
}

void AProjectileActor::SetHomingTarget(AActor* Target)
{
	if (IsValid(Target))
	{
		USceneComponent* HomingComponent = UElectricCastleActorBlueprintFunctionLibrary::FindCollisionComponent(Target);
		if (!HomingComponent)
		{
			HomingComponent = UElectricCastleActorBlueprintFunctionLibrary::FindMeshComponent(Target);
		}
		if (!HomingComponent)
		{
			HomingComponent = Target->GetRootComponent();
		}
		ProjectileMovement->HomingTargetComponent = HomingComponent;
		ProjectileMovement->bIsHomingProjectile = true;
	}
	if (ICombatInterface::IsAlive(Target))
	{
		Cast<ICombatInterface>(Target)->GetOnDeathDelegate().AddDynamic(this, &AProjectileActor::OnTargetDead);
	}
}

void AProjectileActor::OnTargetDead(AActor* DeadActor)
{
	if (bShouldDestroyOnTargetDeath)
	{
		PlayImpactEffect();
		if (HasAuthority())
		{
			Destroy();
		}
	}
	else
	{
		ProjectileMovement->bIsHomingProjectile = false;
		ProjectileMovement->HomingTargetComponent = nullptr;
	}
}

FVector AProjectileActor::GetImpactDirection(const AActor* HitActor) const
{
	if (IsValid(HitActor))
	{
		return HitActor->GetActorLocation() - GetActorLocation();
	}
	return FVector::ZeroVector;
}

TArray<AActor*> AProjectileActor::FindImpactTargets_Implementation()
{
	TArray<FName> TagsToIgnore = ICombatInterface::GetTargetTagsToIgnore(GetOwner());
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	IgnoreActors.Add(GetOwner());
	TArray<AActor*> Targets;
	UElectricCastleAbilitySystemLibrary::GetLiveActorsWithinRadius(this, IgnoreActors, TagsToIgnore, GetActorLocation(), ImpactRadius, Targets);
	return Targets;
}

void AProjectileActor::BeginPlay()
{
	Super::BeginPlay();
	SetReplicateMovement(true);
	SetLifeSpan(LifeSpan);
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AProjectileActor::OnSphereOverlap);
	if (TravelSound)
	{
		TravelSoundComponent = UGameplayStatics::SpawnSoundAttached(TravelSound, GetRootComponent());
	}
	if (TravelSoundComponent)
	{
		TravelSoundComponent->SetSound(TravelSound);
		TravelSoundComponent->Play();
	}
}

void AProjectileActor::OnSphereOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (!IsValidOverlap(OtherActor))
	{
		return;
	}
	PlayImpactEffect();
	if (HasAuthority())
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Hit a target: %s"), *GetName(), *OtherActor->GetName())
		if (UAbilitySystemComponent* OtherAbilitySystem = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
			OtherActor
		))
		{
			DamageEffectParams.DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
			if (FMath::RandRange(1, 100) < DamageEffectParams.KnockbackChance)
			{
				FRotator Rotation = GetActorRotation();
				Rotation.Pitch = 45.f;
				DamageEffectParams.KnockbackForce = Rotation.Vector() * DamageEffectParams.KnockbackForceMagnitude;
			}
			DamageEffectParams.TargetAbilitySystemComponent = OtherAbilitySystem;
			UElectricCastleAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		}
		Destroy();
	}
}

bool AProjectileActor::IsValidOverlap(const AActor* OtherActor) const
{
	const AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();
	if (SourceAvatarActor == OtherActor || OtherActor == this)
	{
		return false;
	}
	if (TagUtils::HasAnyTag(OtherActor, ICombatInterface::GetTargetTagsToIgnore(SourceAvatarActor)))
	{
		return false;
	}
	return true;
}

void AProjectileActor::PlayImpactEffect()
{
	if (bHit)
	{
		// Only play impact effect once
		return;
	}
	// DEVNOTE - this could alternatively be done as a GameplayCue_Burst.
	// This has implications on the number of RPCs, so we should use ExecuteGameplayCue_NonReplicated
	// if we choose to go that route in the future.
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			ImpactSound,
			GetActorLocation(),
			FRotator::ZeroRotator
		);
	}
	if (ImpactEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			ImpactEffect,
			GetActorLocation()
		);
	}
	if (TravelSoundComponent)
	{
		TravelSoundComponent->Stop();
		TravelSoundComponent->DestroyComponent();
	}
	bHit = true;
}

void AProjectileActor::Destroyed()
{
	if (!HasAuthority())
	{
		PlayImpactEffect();
	}
	if (TravelSoundComponent)
	{
		TravelSoundComponent->Stop();
		TravelSoundComponent->DestroyComponent();
	}
	Super::Destroyed();
}
