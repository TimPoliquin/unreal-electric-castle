// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Actor/POI/PointOfInterestActor.h"
#include "GameFramework/Actor.h"
#include "Interaction/HighlightInterface.h"
#include "LevelAssets/Switch/SwitchInterface.h"
#include "Beacon.generated.h"

class USphereComponent;

UENUM(BlueprintType)
enum class EBeaconValidationState : uint8
{
	Incomplete,
	OutOfOrder,
	Complete
};

UCLASS()
class ELECTRICCASTLE_API ABeacon : public APointOfInterestActor, public IHighlightInterface, public ISwitchInterface
{
	GENERATED_BODY()

public:
	ABeacon();
	/** Start IHighlightInterface */
	virtual void HighlightActor_Implementation() override;
	virtual void UnHighlightActor_Implementation() override;
	/** End IHighlightInterface */
	/** Start ISaveInterface */
	FORCEINLINE virtual bool ShouldLoadTransform_Implementation() const override { return false; }
	virtual void PostLoad_Implementation() override;
	/** End ISaveInterface */
	/** Start ISwitchInterface */
	virtual FOnSwitchStatusChangedSignature& GetOnSwitchActivatedDelegate() override { return OnBeaconActivated; };
	virtual FOnSwitchStatusChangedSignature& GetOnSwitchDeactivatedDelegate() override { return OnBeaconReset; };
	virtual bool IsSwitchActive_Implementation() const override { return bIsSwitchActive; };
	virtual void ResetSwitch_Implementation() override;
	/** End ISwitchInterface */
	UPROPERTY(BlueprintAssignable)
	FOnSwitchStatusChangedSignature OnBeaconActivated;
	UPROPERTY(BlueprintAssignable)
	FOnSwitchStatusChangedSignature OnBeaconReset;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static EBeaconValidationState ValidateBeaconActivation(const AActor* BeaconActor, const TArray<ABeacon*>& Beacons, bool IsOrdered);
	UFUNCTION(BlueprintCallable)
	void BroadcastOnBeaconActivated();
	UFUNCTION(BlueprintCallable)
	void BroadcastOnBeaconReset();

protected:
	virtual void BeginPlay() override;

	virtual void HandleInteract_Implementation(AActor* Player) override;
	UFUNCTION(BlueprintNativeEvent)
	void ActivateBeacon(const bool bForced);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> BeaconMesh;

	UPROPERTY(BlueprintReadOnly, SaveGame)
	bool bIsSwitchActive = false;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMaterialInterface> OriginalMaterial;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterialInstance;
};
