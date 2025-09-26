// Copyright Alien Shores


#include "Item/Component/SinusoidalMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


USinusoidalMovementComponent::USinusoidalMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void USinusoidalMovementComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USinusoidalMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bSinusoidalMovement || bRotates)
	{
		const float SinePeriod = 2 * PI / SinePeriodMultiplier;
		RunningTime += DeltaTime;
		if (RunningTime > SinePeriod)
		{
			RunningTime -= SinePeriod;
		}
		Bob(DeltaTime);
	}
}

void USinusoidalMovementComponent::StartSinusoidalMovement()
{
	bSinusoidalMovement = true;
}

void USinusoidalMovementComponent::StartRotation()
{
	bRotates = true;
}

void USinusoidalMovementComponent::Bob(const float DeltaTime)
{
	if (bRotates)
	{
		const FRotator DeltaRotation(0.f, DeltaTime * RotationRate, 0.f);
		GetOwner()->SetActorRelativeRotation(UKismetMathLibrary::ComposeRotators(GetOwner()->GetActorRotation(), DeltaRotation));
	}
	if (bSinusoidalMovement)
	{
		const float Sine = SineAmplitude * FMath::Sin(RunningTime * SinePeriodMultiplier);
		GetOwner()->SetActorLocation(GetOwner()->GetActorLocation() - FVector(0.f, 0.f, PreviousSine) + FVector(0.f, 0.f, Sine));
		PreviousSine = Sine;
	}
}
