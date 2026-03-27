// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InputActionValue.h"
#include "LockOnEvents.h"
#include "Components/ActorComponent.h"
#include "LockOnController.generated.h"


class UInputMappingContext;
class UInputAction;
class AElectricCastlePlayerController;
class UElectricCastleAbilitySystemComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API ULockOnController : public UActorComponent
{
	GENERATED_BODY()

public:
	ULockOnController();
	void HandleAbilitySystemReady(UElectricCastleAbilitySystemComponent* InAbilitySystemComponent);
	void SetPlayerController(AElectricCastlePlayerController* InPlayerController);
	virtual void TickComponent(
		float DeltaTime, enum ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsLockedOn() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetCanLockOn() const;
	UFUNCTION(BlueprintCallable)
	void SetLockOnSupported(const bool bInLockOnSupported);
	UFUNCTION(BlueprintCallable)
	bool ActivateLockOn();
	UFUNCTION(BlueprintCallable)
	void DeactivateLockOn();

	UPROPERTY(BlueprintAssignable)
	FLockOnTargetSignature OnLockOnTarget;
	UPROPERTY(BLueprintAssignable)
	FLockOnReleaseSignature OnLockOnRelease;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UInputMappingContext> LockOnContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UInputAction> SwitchTargetAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxLockOnDistance = 2000.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float LockOnRotationSpeed = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float LockOnPitchOverride = -30.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxWarpingDistance = 50.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bDebug = false;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	bool bLockOnSupported = false;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	bool bLockOnBlocked = false;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	bool bLockOnActivated = false;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TWeakObjectPtr<AActor> TargetActor;

private:
	UPROPERTY()
	TWeakObjectPtr<AElectricCastlePlayerController> PlayerController;
	void UpdateLockOnTarget();
	bool ShouldUpdateTarget() const;
	TArray<AActor*> FindPotentialTargets() const;
	void HandleTagChange_BlockLockOn(FGameplayTag LockOnTag, int Count);
	void UpdateControllerRotation(float DeltaTime);
	void DisengageCurrentTarget();
	void EngageNewTarget(AActor* InTargetActor);
	UFUNCTION()
	void HandleTargetDestroyed(AActor* DestroyedActor);
	void HandleSwitchTargetInput(const FInputActionValue& InputActionValue);
	void UpdateWarpTarget() const;
};
