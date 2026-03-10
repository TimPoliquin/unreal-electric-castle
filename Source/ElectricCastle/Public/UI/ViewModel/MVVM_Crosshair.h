// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Player/Aim/AimController.h"

#include "MVVM_Crosshair.generated.h"

UENUM(BlueprintType)
enum class ECrosshairState : uint8
{
	Hidden,
	Subtle,
	Active,
	Count
};

UENUM(BlueprintType)
enum class ECrosshairStyle : uint8
{
	Default,
	Targeting,
	Count
};

USTRUCT(BlueprintType)
struct FCrosshairStyle
{
	GENERATED_BODY()
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	ECrosshairStyle Style = ECrosshairStyle::Default;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	FLinearColor Color = FLinearColor::White;
	bool operator==(const FCrosshairStyle& CrosshairStyle) const;
};

class AElectricCastlePlayerState;
/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API UMVVM_Crosshair : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InitializeDependencies(const AElectricCastlePlayerState* PlayerState);
	bool GetCanAim() const;
	void SetCanAim(const bool InCanAim);
	bool GetIsAiming() const;
	void SetIsAiming(const bool InIsAiming);
	bool GetIsFiring() const;
	void SetIsFiring(const bool InIsFiring);

	UFUNCTION(BlueprintCallable, BlueprintPure, FieldNotify)
	ECrosshairState GetCrosshairState() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, FieldNotify)
	FCrosshairStyle GetCrosshairStyle() const;

protected:
	UFUNCTION(BlueprintNativeEvent)
	void HandleCanAim();
	UFUNCTION(BlueprintNativeEvent)
	void HandleCannotAim();
	UFUNCTION(BlueprintNativeEvent)
	void HandleAimStart();
	UFUNCTION(BlueprintNativeEvent)
	void HandleAimEnd();
	UFUNCTION(BlueprintNativeEvent)
	void HandleTargetChange(const FTargetChangedPayload& Payload);

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, FieldNotify, Getter, Setter)
	bool CanAim = false;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, FieldNotify, Getter, Setter)
	bool IsAiming = false;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, FieldNotify, Getter, Setter)
	bool IsFiring = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor DefaultCrosshairColor = FLinearColor::White;
	UPROPERTY(VisibleInstanceOnly)
	FCrosshairStyle CrosshairStyle;
};
