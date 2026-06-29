// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "ScalableFloat.h"
#include "Camera/CameraModifier.h"
#include "DirectionalCameraKick.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UDirectionalCameraKick : public UCameraModifier
{
	GENERATED_BODY()

public:
	virtual bool ModifyCamera(float DeltaTime, struct FMinimalViewInfo& InOutPOV) override;
	void Kick(const FVector& InDirection, const FScalableFloat& InMagnitudeCurve, const float InDuration, const bool bInRotate = true);

protected:
	UPROPERTY(BlueprintReadOnly)
	FVector KickDirection;
	UPROPERTY(BlueprintReadOnly)
	float KickTimeRemaining = 0.f;
	UPROPERTY(BlueprintReadOnly)
	FScalableFloat MagnitudeCurve;
	UPROPERTY(BlueprintReadOnly)
	bool bRotate;
	UPROPERTY(BlueprintReadOnly)
	float KickTimeTotal = 0.f;
};
