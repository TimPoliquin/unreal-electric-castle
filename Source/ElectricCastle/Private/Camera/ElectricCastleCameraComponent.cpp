// Copyright Alien Shores


#include "Camera/ElectricCastleCameraComponent.h"

#include "ElectricCastle/ElectricCastleLogChannels.h"

UElectricCastleCameraComponent::UElectricCastleCameraComponent()
{
	CameraTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("CameraTimeline"));
}

void UElectricCastleCameraComponent::MoveToLocation(
	const FVector& Destination,
	const FVector& Direction,
	UCurveFloat* AnimationCurve,
	FOnCameraMoveFinishedSignature* Callback
)
{
	if (AnimationCurve != nullptr)
	{
		CameraTimeline->AddInterpFloat(AnimationCurve, OnTimelineTickDelegate);
		CameraTimeline->SetTimelineFinishedFunc(OnTimelineFinishedDelegate);
		AnimationParams.StartLocation = GetComponentLocation();
		AnimationParams.StartDirection = GetForwardVector();
		AnimationParams.EndDirection = Direction;
		AnimationParams.EndLocation = Destination;
		AnimationParams.Callback = Callback;
		CameraTimeline->PlayFromStart();
	}
}

void UElectricCastleCameraComponent::BeginPlay()
{
	Super::BeginPlay();
	OnTimelineTickDelegate.BindDynamic(this, &UElectricCastleCameraComponent::OnTimelineTick);
	OnTimelineFinishedDelegate.BindDynamic(this, &UElectricCastleCameraComponent::OnTimelineFinished);
}

void UElectricCastleCameraComponent::OnTimelineTick(float Value)
{
	const FVector NewLocation = FMath::Lerp(AnimationParams.StartLocation, AnimationParams.EndLocation, Value);
	const FVector NewDirection = FMath::Lerp(AnimationParams.StartDirection, AnimationParams.EndDirection, Value);
	SetWorldLocation(NewLocation);
	SetWorldRotation(NewDirection.Rotation());
}

void UElectricCastleCameraComponent::OnTimelineFinished()
{
	SetWorldLocation(AnimationParams.EndLocation);
	if (AnimationParams.Callback)
	{
		bool Success = AnimationParams.Callback->ExecuteIfBound();
		if (!Success)
		{
			UE_LOG(
				LogElectricCastle,
				Warning,
				TEXT("Error executing delegate callback in AuraCameraComponent::OnTimelineFinished")
			);
		}
	}
	AnimationParams = FCameraAnimationParams();
}
