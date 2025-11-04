// Copyright Alien Shores


#include "Item/Pickup/ItemPickupBase.h"

#include "NiagaraFunctionLibrary.h"
#include "ElectricCastle/ElectricCastle.h"
#include "Components/CapsuleComponent.h"
#include "Item/Component/SinusoidalMovementComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AItemPickupBase::AItemPickupBase()
{
	PrimaryActorTick.bCanEverTick = false;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
	CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collision Component"));
	CollisionComponent->SetupAttachment(GetRootComponent());
	CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Overlap);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Projectile, ECR_Ignore);
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	MeshComponent->SetupAttachment(CollisionComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetRenderCustomDepth(false);
	SinusoidalMovementComponent = CreateDefaultSubobject<USinusoidalMovementComponent>(TEXT("Sinusoidal Movement Component"));
}

// Called when the game starts or when spawned
void AItemPickupBase::BeginPlay()
{
	Super::BeginPlay();
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AItemPickupBase::OnBeginOverlap);
	CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &AItemPickupBase::OnEndOverlap);
	MeshComponent->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
	PlaySpawnEffect();
}

void AItemPickupBase::HighlightActor_Implementation()
{
	MeshComponent->SetRenderCustomDepth(true);
}

void AItemPickupBase::UnHighlightActor_Implementation()
{
	MeshComponent->SetRenderCustomDepth(false);
}

bool AItemPickupBase::SetMoveToLocation_Implementation(FVector& OutDestination)
{
	OutDestination = GetActorLocation();
	return true;
}

void AItemPickupBase::PlaySpawnEffect_Implementation()
{
	if (SpawnSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SpawnSound, GetActorLocation());
	}
	if (SpawnEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, SpawnEffect, GetActorLocation(), GetActorRotation());
	}
}

void AItemPickupBase::PlayPickupEffect_Implementation(AActor* PickupActor, const bool bAutoDestroy)
{
	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			PickupActor,
			PickupSound,
			PickupActor->GetActorLocation());
	}
	if (PickupEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(
			PickupEffect,
			PickupActor->GetRootComponent(),
			NAME_None,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::Type::SnapToTarget,
			true,
			true
		);
	}
	if (bAutoDestroy)
	{
		Destroy();
	}
}
