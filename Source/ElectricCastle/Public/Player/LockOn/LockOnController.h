// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayTagContainer.h"
#include "InputActionValue.h"
#include "LockOnEvents.h"
#include "Components/ActorComponent.h"
#include "LockOnController.generated.h"


class UElectricCastleAttributeSet;
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
	bool IsHardLockedOn() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasLockOnTarget() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetCanLockOn() const;
	UFUNCTION(BlueprintCallable)
	void SetLockOnSupported(const bool bInLockOnSupported);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	AActor* GetLockOnTarget() const;

	UPROPERTY(BlueprintAssignable)
	FLockOnTargetSignature OnLockOnTargetChanged;
	UPROPERTY(BlueprintAssignable)
	FLockOnTargetSignature OnLockOnLevelChanged;

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
	bool bDebug = false;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	bool bLockOnSupported = false;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	bool bLockOnBlocked = false;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TWeakObjectPtr<AActor> TargetActor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ELockOnLevel LockOnLevel = ELockOnLevel::Soft;

private:
	UPROPERTY()
	TWeakObjectPtr<AElectricCastlePlayerController> PlayerController;
	UFUNCTION()
	void HandleTagChange_Effect_LockedOn(FGameplayTag LockedOnTag, int Count);
	UFUNCTION()
	bool ActivateHardLockOn();
	UFUNCTION()
	void DeactivateHardLockOn();

	void UpdateLockOnTarget();
	bool ShouldUpdateTarget() const;
	void HandleTagChange_BlockLockOn(FGameplayTag LockOnTag, int Count);
	void UpdateControllerRotation(float DeltaTime);
	void DisengageCurrentTarget();
	void EngageNewTarget(AActor* InTargetActor);
	UFUNCTION()
	void HandleTargetDestroyed(AActor* DestroyedActor);
	void HandleSwitchTargetInput(const FInputActionValue& InputActionValue);
	void UpdateWarpTarget() const;
	void SetLockOnLevel(const ELockOnLevel InLockOnLevel);
	UFUNCTION()
	void HandleGameDataLoaded();

	static const float REDUCED_TICK;
};
