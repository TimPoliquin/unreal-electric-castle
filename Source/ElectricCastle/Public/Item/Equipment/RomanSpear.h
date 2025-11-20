// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "BasicEquipmentActor.h"
#include "UObject/Object.h"
#include "RomanSpear.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API ARomanSpear : public ABasicEquipmentActor
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaSeconds) override;
};
