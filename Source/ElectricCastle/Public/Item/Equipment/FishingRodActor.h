// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "BasicEquipmentActor.h"
#include "Item/Equipment/FishingBobActor.h"
#include "FishingRodActor.generated.h"

class UCableComponent;
class AFishingBobActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFishingBobLandedSignature);

UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API AFishingRodActor : public ABasicEquipmentActor
{
	GENERATED_BODY()

public:
	AFishingRodActor();

	UFUNCTION(BlueprintCallable)
	FVector GetRodTipLocation() const;
	UFUNCTION(BlueprintCallable)
	FRotator GetRodTipRotation() const;
	UFUNCTION(BlueprintCallable)
	void Cast(const FVector& Destination);
	void Return();
	UPROPERTY(BlueprintAssignable)
	FOnFishingBobLandedSignature OnFishingBobLandedDelegate;
	virtual void Unequip_Implementation(AActor* InOwner) override;
	AFishingBobActor* GetFishingBob() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Fishing Rod")
	TSubclassOf<AFishingBobActor> FishingBobClass;
	UPROPERTY(BlueprintReadOnly, Category = "Fishing Rod")
	TObjectPtr<AFishingBobActor> BobActor;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fishing Rod")
	TObjectPtr<UCableComponent> CableComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fishing Rod")
	FName RodTipSocket = FName(TEXT("RodTip"));


	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnFishingBobStateChanged(EFishingBobState FishingState);
	AFishingBobActor* CreateFishingBob();
};
