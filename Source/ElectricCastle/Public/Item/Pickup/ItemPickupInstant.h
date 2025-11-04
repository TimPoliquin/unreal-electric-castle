// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "ItemPickupBase.h"
#include "TieredItemInterface.h"
#include "ItemPickupInstant.generated.h"

UCLASS()
class ELECTRICCASTLE_API AItemPickupInstant : public AItemPickupBase, public ITieredItemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItemPickupInstant();
	/** Start ITieredItemInterface **/
	virtual int32 GetItemLevel_Implementation() const override;
	virtual void SetItemLevel_Implementation(int32 Level) override;
	/** End ITieredItemInterface **/

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UApplyGameplayEffectComponent> EffectComponent;

	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                            const FHitResult& SweepResult) override;
	virtual void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
};
