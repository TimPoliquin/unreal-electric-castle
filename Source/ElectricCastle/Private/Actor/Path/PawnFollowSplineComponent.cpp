// Copyright Alien Shores


#include "Actor/Path/PawnFollowSplineComponent.h"

#include "Components/SplineComponent.h"
#include "GameFramework/MovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


UPawnFollowSplineComponent::UPawnFollowSplineComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UPawnFollowSplineComponent::BeginPlay()
{
	Super::BeginPlay();
	MovementComponent = GetOwner()->FindComponentByClass<UPawnMovementComponent>();
}

void UPawnFollowSplineComponent::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	MoveAlongSpline(DeltaTime);
}

void UPawnFollowSplineComponent::FollowSpline(AActor* SplineActor)
{
	SplineComponent = Cast<USplineComponent>(SplineActor->GetComponentByClass(USplineComponent::StaticClass()));
}


void UPawnFollowSplineComponent::MoveAlongSpline(float DeltaTime)
{
	if (!IsValid(MovementComponent) || !SplineComponent.IsValid())
	{
		return;
	}
	DistanceAlongSpline = FMath::Modulo(DistanceAlongSpline + MovementComponent.Get()->GetMaxSpeed() * DeltaTime, SplineComponent.Get()->GetSplineLength());
	const FVector& ForwardVector = UKismetMathLibrary::GetForwardVector(SplineComponent.Get()->GetRotationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::Local));
	MovementComponent.Get()->AddInputVector(ForwardVector * MovementComponent.Get()->GetMaxSpeed());
}
