// Copyright Alien Shores


#include "Item/Equipment/FishingBobActor.h"

#include "Actor/ElectricCastleActorBlueprintFunctionLibrary.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFishingBobActor::AFishingBobActor()
{
	PrimaryActorTick.bCanEverTick = true;
	Collider = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Collider->SetGenerateOverlapEvents(true);
	Collider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Collider->SetCollisionResponseToAllChannels(ECR_Ignore);
	Collider->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	SetRootComponent(Collider);
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(Collider);
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	ProjectileMovementComponent->InitialSpeed = 0;
	ProjectileMovementComponent->MaxSpeed = 0;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;
}

void AFishingBobActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	switch (FishingState)
	{
	case EFishingBobState::None:
		break;
	case EFishingBobState::Casting:
		HandleCastingTick(DeltaTime);
		break;
	case EFishingBobState::Bobbing:
		HandleBobbingTick(DeltaTime);
		break;
	case EFishingBobState::Lured:
		HandleLuredTick(DeltaTime);
		break;
	case EFishingBobState::Biting:
		HandleBitingTick(DeltaTime);
		break;
	case EFishingBobState::Reeling:
		HandleReelingTick(DeltaTime);
	}
}

void AFishingBobActor::Launch(const FVector& InDestination)
{
	Destination = InDestination;
	SetActorHiddenInGame(false);
	SetActorTickEnabled(true);
	SetActorEnableCollision(true);
	SetFishingState(EFishingBobState::Casting);
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetActorRotation(FRotator::ZeroRotator);
	FVector LaunchVelocity;
	UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, LaunchVelocity, GetActorLocation(), InDestination);
	if (bDrawDebug)
	{
		DrawDebugSphere(GetWorld(), InDestination, 50.f, 12, FColor::Red, true);
		DrawDebugDirectionalArrow(
			GetWorld(),
			GetActorLocation(),
			GetActorLocation() + LaunchVelocity,
			LaunchVelocity.Length(),
			FColor::Black,
			false,
			10.f,
			0,
			1.f
		);
	}
	ProjectileMovementComponent->SetVelocityInLocalSpace(LaunchVelocity);
	ProjectileMovementComponent->ProjectileGravityScale = 1.f;
	Collider->SetEnableGravity(true);
	Mesh->SetEnableGravity(true);
	Mesh->SetVisibility(true);
}

void AFishingBobActor::Lured()
{
	SetFishingState(EFishingBobState::Lured);
}

void AFishingBobActor::Biting()
{
	SetFishingState(EFishingBobState::Biting);
	SetActorLocation(Destination + FVector::DownVector * BobbingAmplitude * 3.f);
}

void AFishingBobActor::Cancel()
{
	SetFishingState(EFishingBobState::None);
	Destination = FVector::ZeroVector;
	OnFishingStateChanged.Clear();
}

void AFishingBobActor::Return(AActor* InOwner, const FName& SocketName)
{
	SetOwner(InOwner);
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
	SetActorEnableCollision(false);
	SetFishingState(EFishingBobState::None);
	AttachToActor(InOwner, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
	ProjectileMovementComponent->StopMovementImmediately();
	ProjectileMovementComponent->ProjectileGravityScale = 0;
	Collider->SetEnableGravity(false);
	Mesh->SetEnableGravity(false);
	Mesh->SetVisibility(false);
}

void AFishingBobActor::HandleCastingTick(float DeltaTime)
{
	if (FVector::Distance(GetActorLocation(), Destination) <= DestinationThreshold || GetActorLocation().Z < Destination
		.Z)
	{
		ProjectileMovementComponent->StopMovementImmediately();
		ProjectileMovementComponent->ProjectileGravityScale = 0.f;
		Collider->SetEnableGravity(false);
		Mesh->SetEnableGravity(false);
		SetActorLocation(Destination);
		BobbingTime = 0.f;
		SetFishingState(EFishingBobState::Bobbing);
	}
	else if (bDrawDebug)
	{
		DrawDebugSphere(GetWorld(), GetActorLocation(), 10.f, 12, FColor::White, true, 2.f, 0, 1.f);
	}
}

void AFishingBobActor::HandleBobbingTick(const float DeltaTime)
{
	UElectricCastleActorBlueprintFunctionLibrary::SinusoidalMovement(
		this,
		DeltaTime,
		BobbingTime,
		BobbingPeriodMultiplier,
		BobbingAmplitude,
		Destination
	);
}

void AFishingBobActor::HandleLuredTick(float DeltaTime)
{
	UElectricCastleActorBlueprintFunctionLibrary::SinusoidalMovement(
		this,
		DeltaTime,
		BobbingTime,
		BobbingPeriodMultiplier * 5.f,
		BobbingAmplitude * 2.f,
		Destination + FVector::DownVector * BobbingAmplitude * 1.5
	);
}

void AFishingBobActor::HandleBitingTick(float DeltaTime)
{
	// Maybe nothing to do here
}

void AFishingBobActor::HandleReelingTick(float DeltaTime)
{
}
