// Copyright Alien Shores


#include "Actor/Physics/BasicGravityComponent.h"

#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

UBasicGravityComponent::UBasicGravityComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBasicGravityComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBasicGravityComponent::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!GetOwner())
	{
		return;
	}

	FHitResult GroundHit;
	bIsGrounded = PerformGroundCheck(GroundHit);

	if (bIsGrounded)
	{
		ResolveGroundCollision(GroundHit);
	}
	else
	{
		ApplyGravity(DeltaTime);
	}

	// Apply movement
	const FVector MoveDelta = FVector(0.f, 0.f, VerticalVelocity * DeltaTime);
	GetOwner()->AddActorWorldOffset(MoveDelta, true);
}

void UBasicGravityComponent::Activate(const bool bReset)
{
	Super::Activate(bReset);
	SetComponentTickEnabled(true);
}

void UBasicGravityComponent::Deactivate()
{
	Super::Deactivate();
	SetComponentTickEnabled(false);
}

bool UBasicGravityComponent::PerformGroundCheck(FHitResult& OutHit) const
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return false;
	}

	const FVector Start = Owner->GetActorLocation();
	const FVector End = Start + FVector(0.f, 0.f, -GroundCheckDistance);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);

	return GetWorld()->LineTraceSingleByChannel(
		OutHit,
		Start,
		End,
		ECC_Visibility,
		Params
	);
}

void UBasicGravityComponent::ApplyGravity(const float DeltaTime)
{
	VerticalVelocity += GravityAcceleration * DeltaTime;
	VerticalVelocity = FMath::Max(VerticalVelocity, TerminalVelocity);
}

void UBasicGravityComponent::ResolveGroundCollision(const FHitResult& Hit)
{
	// Snap to ground
	FVector OwnerLoc = GetOwner()->GetActorLocation();
	OwnerLoc.Z = Hit.ImpactPoint.Z;
	GetOwner()->SetActorLocation(OwnerLoc, true);
	// Reset vertical velocity
	VerticalVelocity = 0.f;
	OnLandedDelegate.Broadcast();
	if (bAutoDeactivate)
	{
		Deactivate();
	}
}
