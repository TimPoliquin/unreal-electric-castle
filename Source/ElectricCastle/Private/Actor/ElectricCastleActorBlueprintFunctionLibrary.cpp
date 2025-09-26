// Copyright Alien Shores


#include "Actor/ElectricCastleActorBlueprintFunctionLibrary.h"

#include "Actor/CollidableInterface.h"
#include "Components/ShapeComponent.h"

void UElectricCastleActorBlueprintFunctionLibrary::SinusoidalMovement(
	AActor* Actor,
	const float DeltaSeconds,
	float& RunningTime,
	const float PeriodMultiplier,
	const float Amplitude,
	const FVector& InitialLocation
)
{
	const float SinePeriod = 2 * PI / PeriodMultiplier;
	RunningTime += DeltaSeconds;
	if (RunningTime > SinePeriod)
	{
		RunningTime -= SinePeriod;
	}
	const float Sine = Amplitude * FMath::Sin(RunningTime * PeriodMultiplier);
	Actor->SetActorLocation(InitialLocation + FVector(0.f, 0.f, Sine));
}

void UElectricCastleActorBlueprintFunctionLibrary::DisableActor(AActor* Actor)
{
	Actor->SetActorHiddenInGame(true);
	Actor->SetActorTickEnabled(false);
	Actor->SetActorEnableCollision(false);
}

UShapeComponent* UElectricCastleActorBlueprintFunctionLibrary::FindCollisionComponent(const AActor* Actor)
{
	if (!IsValid(Actor))
	{
		return nullptr;
	}
	if (Actor->Implements<UCollidableInterface>())
	{
		return Cast<ICollidableInterface>(Actor)->GetPrimaryCollisionComponent();
	}
	return nullptr;
}

UMeshComponent* UElectricCastleActorBlueprintFunctionLibrary::FindMeshComponent(const AActor* Actor)
{
	return Actor->FindComponentByClass<UMeshComponent>();
}
