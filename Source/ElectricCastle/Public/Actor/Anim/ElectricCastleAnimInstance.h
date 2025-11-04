// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ElectricCastleAnimInstance.generated.h"

class UCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UElectricCastleAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector Velocity;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float GroundSpeed = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bIsMoving = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bIsFalling = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bIsDead = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bIsAiming = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bIsTurning = false;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float SmoothedYawDelta; // Smoothed for blend space
	// Previous frame data
	float PreviousYaw;


	UFUNCTION(BlueprintNativeEvent)
	void UpdateVelocity();
	UFUNCTION(BlueprintNativeEvent)
	void UpdateRotation(const float DeltaSeconds);
	UFUNCTION(BlueprintNativeEvent)
	void UpdateIsFalling();
	UFUNCTION(BlueprintNativeEvent)
	void UpdateIsDead();
	UFUNCTION(BlueprintNativeEvent)
	void UpdateIsAiming();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	ACharacter* TryGetCharacterOwner();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UCharacterMovementComponent* TryGetCharacterMovementComponent();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	AController* TryGetController();

private:
	UPROPERTY()
	TObjectPtr<ACharacter> CharacterOwner;
	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> CharacterMovementComponent;
	UPROPERTY()
	TObjectPtr<AController> CharacterController;
};
