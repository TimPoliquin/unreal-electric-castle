// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "ItemPickupInventory.h"
#include "TreasurePickup.generated.h"

UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API ATreasurePickup : public AItemPickupInventory
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATreasurePickup();

	static ATreasurePickup* SpawnTreasure(const AActor* WorldContextObject, const FVector& Location, const float Value, const bool bRandomize = true);
};
