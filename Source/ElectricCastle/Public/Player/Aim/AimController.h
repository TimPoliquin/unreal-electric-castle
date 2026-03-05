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

	UPROPERTY(BlueprintAssignable)
	FCanAimSignature OnCanAim;
	UPROPERTY(BlueprintAssignable)
	FCanAimSignature OnCannotAim;
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
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsAiming() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasTarget() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	AActor* GetTarget() const;

	void TraceForTarget(const FVector& PlayerLocation, const FVector& CameraLocation, const FRotator& CameraRotation);

protected:
	void SetCanAim(bool bInCanAim);
	void SetIsAiming(bool bInIsAiming);
	void SetTarget(AActor* InTarget);
	void ClearTarget();
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Aim")
	bool bCanAim = false;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Aim")
	bool bIsAiming = false;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Aim")
	bool bIsFiring = false;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Aim")
	float TraceDistance = 1000.f;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Aim")
	float TraceRadius = 10.f;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Aim")
	TWeakObjectPtr<AActor> Target;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aim")
	bool bDebug = false;

private:
	FActiveGameplayEffectHandle AimingEffectHandle;
	static FVector CalculateTraceStartLocation(const FVector& PlayerLocation, const FVector& CameraLocation, const FVector& TraceEnd);
	void HandleTagChange_BlockAim(FGameplayTag BlockAimTag, int Count);
};
