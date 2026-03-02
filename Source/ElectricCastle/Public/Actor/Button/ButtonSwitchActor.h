// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Actor/Switch/SwitchInterface.h"
#include "ButtonSwitchActor.generated.h"

class UBoxComponent;

UCLASS(Abstract)
class ELECTRICCASTLE_API AButtonSwitchActor : public AActor, public ISwitchInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AButtonSwitchActor();
	/** Start ISwitchInterface **/
	virtual bool IsSwitchActive_Implementation() const override { return bIsActivated; };
	virtual void ResetSwitch_Implementation() override;
	virtual FOnSwitchStatusChangedSignature& GetOnSwitchActivatedDelegate() override { return OnActivatedDelegate; };
	virtual FOnSwitchStatusChangedSignature& GetOnSwitchDeactivatedDelegate() override { return OnDeactivatedDelegate; };
	/** End ISwitchInterface **/

	UPROPERTY(BlueprintAssignable)
	FOnSwitchStatusChangedSignature OnActivatedDelegate;
	UPROPERTY(BlueprintAssignable)
	FOnSwitchStatusChangedSignature OnDeactivatedDelegate;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool ActivateSwitch();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool DeactivateSwitch();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateSwitchMesh_Activate(UStaticMeshComponent* InSwitchMesh);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateSwitchMesh_Deactivate(UStaticMeshComponent* InSwitchMesh);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> BaseMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> SwitchMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> SwitchDetector;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Button")
	bool bIsActivated = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button")
	bool bIsActivationLocked = false;

private:
	UFUNCTION()
	void OnSwitchDetectorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                  const FHitResult& SweepResult);
	UFUNCTION()
	void OnSwitchDetectorEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	bool CanDeactivateSwitch(const UPrimitiveComponent* DetectionComponent) const;
	static void InitializeMeshCollisionResponse(UStaticMeshComponent* MeshComponent);
};
