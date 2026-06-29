// Copyright Alien Shores


#include "Actor/Camera/CameraMovementModifier.h"

#include "Camera/CameraComponent.h"

void UCameraMovementModifier::GetPlayerMovementInput_Implementation(
	FVector& OutPlayerForwardDirection,
	FVector& OutPlayerRightDirection
) const
{
	if (!IsValid(GetCameraComponent()))
	{
		return;
	}
	const FRotator Rotation = GetCameraComponent()->GetComponentRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
	OutPlayerForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	OutPlayerRightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
}

void UCameraMovementModifier::SetCameraComponent_Implementation(UCameraComponent* InCameraComponent)
{
	CameraComponent = InCameraComponent;
}
