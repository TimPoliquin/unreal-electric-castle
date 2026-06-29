// Copyright Alien Shores


#include "Actor/Camera/TopDownCameraMovementModifier.h"

#include "Camera/CameraComponent.h"

void UTopDownCameraMovementModifier::GetPlayerMovementInput_Implementation(
	FVector& OutPlayerForwardDirection,
	FVector& OutPlayerRightDirection
) const
{
	if (!IsValid(GetCameraComponent()))
	{
		return;
	}
	const FRotator CamRot = GetCameraComponent()->GetComponentRotation();
	const FRotationMatrix CamMatrix(CamRot);
	// For top-down: screen "up" = camera's up vector (Z axis)
	OutPlayerForwardDirection = CamMatrix.GetUnitAxis(EAxis::Z); // Up
	OutPlayerRightDirection = CamMatrix.GetUnitAxis(EAxis::Y); // Right
	// Flatten to horizontal plane
	OutPlayerForwardDirection.Z = 0.f;
	OutPlayerRightDirection.Z = 0.f;
	OutPlayerForwardDirection.Normalize();
	OutPlayerRightDirection.Normalize();
}
