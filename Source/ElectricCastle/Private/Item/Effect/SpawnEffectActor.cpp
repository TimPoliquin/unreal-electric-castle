// Copyright Alien Shores


#include "Item/Effect/SpawnEffectActor.h"

#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ASpawnEffectActor::ASpawnEffectActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void ASpawnEffectActor::BeginPlay()
{
	Super::BeginPlay();
}


void ASpawnEffectActor::PlaySpawnEffect_Implementation(const FAuraSpawnEffectParams& SpawnParams)
{
	// nothing to do here right now.
}

FTransform ASpawnEffectActor::CalculateTransform(const FAuraSpawnEffectParams& SpawnParams, const float Alpha, const float HeightAlpha, const float ScaleAlpha)
{
	const FVector AlphaLocation = FMath::Lerp(SpawnParams.SpawnTransform.GetLocation(), SpawnParams.TargetTransform.GetLocation(), Alpha);
	const FVector AdjustedLocation = AlphaLocation + FVector(0, 0, SpawnParams.HeightMultiplier * HeightAlpha);
	const FVector TraceEnd = AdjustedLocation - FVector(0, 0, 1000);
	FVector ActorOrigin;
	FVector BoxExtent;
	TArray<AActor*> ChildActors;
	GetAttachedActors(ChildActors, true);
	UGameplayStatics::GetActorArrayBounds(ChildActors, true, ActorOrigin, BoxExtent);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(Owner);
	QueryParams.AddIgnoredActors(ChildActors);
	if (FHitResult GroundHit; GetWorld()->LineTraceSingleByChannel(GroundHit, AdjustedLocation, TraceEnd, ECC_Visibility, QueryParams))
	{
		if (GroundHit.ImpactPoint.Z < AdjustedLocation.Z - BoxExtent.Z)
		{
			// if the ground is lower than this, allow the move
			FTransform NewTransform;
			NewTransform.SetLocation(AdjustedLocation);
			NewTransform.SetScale3D(FVector(ScaleAlpha, ScaleAlpha, ScaleAlpha));
			return NewTransform;
		}
	}
	// no ground beneath the object -- stop moving!
	return GetTransform();
}
