// Copyright Alien Shores


#include "Camera/Kick/DirectionalCameraKick.h"

bool UDirectionalCameraKick::ModifyCamera(float DeltaTime, struct FMinimalViewInfo& InOutPOV)
{
	if (KickTimeRemaining > 0.f)
	{
		const float KickAlpha = KickTimeRemaining / KickTimeTotal;
		const float KickStrength = MagnitudeCurve.GetValueAtLevel(1.f - KickAlpha);
		InOutPOV.Location += KickDirection * KickStrength;
		KickTimeRemaining -= DeltaTime;
		if (bRotate)
		{
			InOutPOV.Rotation.Pitch += KickDirection.Z * KickStrength * 0.5f;
			InOutPOV.Rotation.Yaw += KickDirection.Y * KickStrength * 0.5f;
			InOutPOV.Rotation.Roll += KickDirection.X * KickStrength * 0.2f;
		}
	}
	return false;
}

void UDirectionalCameraKick::Kick(const FVector& InDirection, const FScalableFloat& InMagnitudeCurve, const float InDuration, const bool bInRotate)
{
	KickDirection = InDirection;
	KickTimeRemaining = InDuration;
	KickTimeTotal = InDuration;
	MagnitudeCurve = InMagnitudeCurve;
	bRotate = bInRotate;
}
