// Copyright Alien Shores


#include "Actor/Projectile/ProjectileActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "Actor/ElectricCastleActorBlueprintFunctionLibrary.h"
#include "Actor/Cinematic/CinematicHandlerComponent.h"
#include "Actor/Pool/PoolManagerComponent.h"
#include "ElectricCastle/ElectricCastle.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Interaction/CombatInterface.h"
#include "Net/UnrealNetwork.h"
#include "Utils/TagUtils.h"

AProjectileActor::AProjectileActor()
{
	PrimaryActorTick.bCanEverTick = false;
	// DEVNOTE: Ensures that the state is replicated from server -> client
	bReplicates = true;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
	CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Sphere"));
	CollisionComponent->SetupAttachment(GetRootComponent());
	CollisionComponent->SetCollisionObjectType(ECC_Projectile);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	CollisionComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 550.f;
	ProjectileMovement->MaxSpeed = 550.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	CinematicHandlerComponent = CreateDefaultSubobject<UCinematicHandlerComponent>(TEXT("CinematicHandlerComponent"));
	PoolManagerComponent = CreateDefaultSubobject<UPoolManagerComponent>(TEXT("PoolManagerComponent"));
}

void AProjectileActor::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AProjectileActor, DamageEffectParams);
	DOREPLIFETIME(AProjectileActor, ExplosionDamageConfig);
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
			ReturnToPoolOrDestroy(this);
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

TArray<AActor*> AProjectileActor::FindExplosionTargets_Implementation()
{
	TArray<FName> TagsToIgnore = DamageEffectParams.IgnoreTags;
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	IgnoreActors.Add(GetOwner());
	TArray<AActor*> Targets;
	UElectricCastleAbilitySystemLibrary::GetLiveActorsWithinRadius(
		this,
		IgnoreActors,
		TagsToIgnore,
		GetActorLocation(),
		ImpactRadius,
		Targets,
		bDebug
	);
	return Targets;
}

void AProjectileActor::SetupExplosionConfig(
	const TSubclassOf<UGameplayEffect>& InExplosionDamageEffectClass,
	const FElectricCastleDamageConfig& InExplosionDamageConfig
)
{
	ExplosionDamageEffectClass = InExplosionDamageEffectClass;
	ExplosionDamageConfig = InExplosionDamageConfig;
}

void AProjectileActor::Fire_Implementation()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ProjectileMovement->Velocity = ProjectileMovement->InitialSpeed * GetActorForwardVector();
	ProjectileMovement->Activate(true);
	PoolManagerComponent->StartManualAutoReturn();
}

void AProjectileActor::ApplyDamageEffectParams_Implementation(const FDamageEffectParams& InDamageEffectParams)
{
	DamageEffectParams = InDamageEffectParams;
}

UPoolManagerComponent* AProjectileActor::GetPoolManager_Implementation() const
{
	return PoolManagerComponent;
}

void AProjectileActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	PoolManagerComponent->OnBeginRetrieve.AddUniqueDynamic(this, &AProjectileActor::OnPool_BeginRetrieve);
	PoolManagerComponent->OnFinishRetrieve.AddUniqueDynamic(this, &AProjectileActor::OnPool_FinishRetrieve);
	PoolManagerComponent->OnReturnToPool.AddDynamic(this, &AProjectileActor::OnPool_Returned);
}

void AProjectileActor::OnSphereOverlap_Implementation(
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
		if (bDebug)
		{
			UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Hit a target: %s"), *GetName(), *OtherActor->GetName())
		}
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
		if (bExplodeOnImpact)
		{
			Explode();
		}
		ReturnToPoolOrDestroy(this);
	}
}

bool AProjectileActor::IsValidOverlap(const AActor* OtherActor) const
{
	const AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent ? DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor() : GetOwner();
	if (SourceAvatarActor == OtherActor || OtherActor == this)
	{
		return false;
	}
	if (TagUtils::HasAnyTag(OtherActor, DamageEffectParams.IgnoreTags))
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

void AProjectileActor::OverrideDamageMagnitude(const float InDamageMagnitudeOverride)
{
	DamageEffectParams.DamageMagnitude = InDamageMagnitudeOverride;
}

void AProjectileActor::OnPool_BeginRetrieve_Implementation(const FSpawnPoolEventPayload& Payload)
{
	// Nothing to do here - just in case!
}

void AProjectileActor::OnPool_FinishRetrieve_Implementation(const FSpawnPoolEventPayload& Payload)
{
	bHit = false;
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &AProjectileActor::OnSphereOverlap);
	SetActorTickEnabled(true);
	SetReplicateMovement(true);
	if (bAutoLaunchProjectile)
	{
		ProjectileMovement->Velocity = ProjectileMovement->InitialSpeed * GetActorForwardVector();
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
	else
	{
		ProjectileMovement->Velocity = FVector::ZeroVector;
	}
}

void AProjectileActor::OnPool_Returned_Implementation(const FSpawnPoolEventPayload& Payload)
{
	// Stop sounds, clear timers, etc.
	SetActorTickEnabled(false);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (TravelSoundComponent)
	{
		TravelSoundComponent->Stop();
		TravelSoundComponent->DestroyComponent();
	}
	if (ProjectileMovement->HomingTargetComponent.IsValid())
	{
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(ProjectileMovement->HomingTargetComponent->GetOwner()))
		{
			CombatInterface->GetOnDeathDelegate().RemoveAll(this);
		}
	}
	if (CinematicHandlerComponent)
	{
		CinematicHandlerComponent->Cleanup();
	}
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionComponent->OnComponentBeginOverlap.RemoveAll(this);
	ProjectileMovement->StopMovementImmediately();
	ProjectileMovement->Velocity = FVector::ZeroVector;
	ProjectileMovement->HomingTargetComponent.Reset();
}

void AProjectileActor::Explode_Implementation()
{
	for (AActor* TargetActor : FindExplosionTargets())
	{
		ExplodeOnTarget(TargetActor);
	}
}

void AProjectileActor::ExplodeOnTarget_Implementation(AActor* TargetActor)
{
	if (!IsValid(TargetActor))
	{
		return;
	}
	const FDamageEffectParams& ExplosionDamageEffectParams =
		UElectricCastleAbilitySystemLibrary::MakeCustomDamageEffectParams(
			GetOwner(),
			TargetActor,
			ExplosionDamageEffectClass,
			ExplosionDamageConfig,
			DamageEffectParams.AbilityLevel,
			DamageEffectParams.AbilityAssetTags,
			GetActorLocation()
		);
	UElectricCastleAbilitySystemLibrary::ApplyDamageEffect(
		ExplosionDamageEffectParams
	);
}
