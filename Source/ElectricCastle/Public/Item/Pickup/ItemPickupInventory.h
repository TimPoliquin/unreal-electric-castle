// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "ItemPickupBase.h"
#include "ItemPickupInventory.generated.h"

UCLASS()
class ELECTRICCASTLE_API AItemPickupInventory : public AItemPickupBase
{
	GENERATED_BODY()

public:
	AItemPickupInventory();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item", meta=(ExposeOnSpawn))
	int32 Count = 1;
	UFUNCTION(BlueprintCallable, Category="Item")
	void ActivatePickup(AActor* PickupActor);

	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                            const FHitResult& SweepResult) override;
};
