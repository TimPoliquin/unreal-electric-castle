// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"

#include "AimController.generated.h"

class UElectricCastleAbilitySystemComponent;

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FTargetChangedPayload
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	AActor* Target = nullptr;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCanAimSignature);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAimStateChangeSignature);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTargetChangedSignature, const FTargetChangedPayload&, Payload);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API UAimController : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAimController();

	void HandleAbilitySystemReady(UElectricCastleAbilitySystemComponent* InAbilitySystemComponent);
	void SetTraceParams(const float TraceDistance, const float TraceRadius);

	UPROPERTY(BlueprintAssignable)
	FCanAimSignature OnShowCrosshair;
	UPROPERTY(BlueprintAssignable)
	FCanAimSignature OnHideCrosshair;
	UPROPERTY(BlueprintAssignable)
	FAimStateChangeSignature OnAimStart;
	UPROPERTY(BlueprintAssignable)
	FAimStateChangeSignature OnAimEnd;
	UPROPERTY(BlueprintAssignable)
	FTargetChangedSignature OnTargetChanged;

	UFUNCTION(BlueprintCallable)
	void AimStart();
	UFUNCTION(BlueprintCallable)
	void AimEnd();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetCanAim() const;
	UFUNCTION(BlueprintCallable)
	void SetCanAim(bool bInCanAim);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetHideCrosshair() const;
	UFUNCTION(BlueprintCallable)
	void SetHideCrosshair(const bool bInHideCrosshair);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsAiming() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasTarget() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	AActor* GetTarget() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetHitLocation() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FRotator CalculateRotationToFaceAimTarget(const FVector& Location) const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetAimDirection(const FVector& Location) const;

	void TraceForTarget(const FVector& PlayerLocation, const FVector& CameraLocation, const FRotator& CameraRotation);
	FHitResult GetTraceResult() const { return TraceResult; }

protected:
	void SetIsAiming(bool bInIsAiming);
	void SetTarget(AActor* InTarget) const;
	void ClearTarget(const AActor* InOldTarget) const;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Aim")
	bool bHideCrosshair = true;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Aim")
	bool bCanAim = false;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Aim")
	bool bIsAiming = false;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Aim")
	bool bIsFiring = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aim")
	float TraceDistance = 1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aim")
	float TraceRadius = 10.f;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Aim")
	FHitResult TraceResult;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Aim")
	FVector TraceEndLocation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aim")
	bool bDebug = false;

private:
	FActiveGameplayEffectHandle AimingEffectHandle;
	static FVector CalculateTraceStartLocation(const FVector& PlayerLocation, const FVector& CameraLocation, const FVector& TraceEnd);
	void HandleTagChange_BlockAim(FGameplayTag BlockAimTag, int Count);
	void BroadcastCrosshairEvent();
	void ClearTrace();
};
