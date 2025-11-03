// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "EquipmentActor.h"
#include "UObject/Object.h"
#include "RomanSpear.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API ARomanSpear : public AEquipmentActor
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaSeconds) override;
};
