// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "CameraMovementModifier.h"
#include "TopDownCameraMovementModifier.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UTopDownCameraMovementModifier : public UCameraMovementModifier
{
	GENERATED_BODY()

public:
	virtual void GetPlayerMovementInput_Implementation(
		FVector& OutPlayerForwardDirection,
		FVector& OutPlayerRightDirection
	) const override;
};
