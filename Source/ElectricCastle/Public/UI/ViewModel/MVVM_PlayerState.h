// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "AbilitySystem/AttributeChangeDelegates.h"
#include "Player/Form/PlayerFormChangeComponent.h"
#include "UObject/Object.h"
#include "MVVM_PlayerState.generated.h"

class AElectricCastlePlayerState;
/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UMVVM_PlayerState : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	int32 GetPlayerIndex() const;
	void SetPlayerIndex(const int32 InPlayerIndex);
	int32 GetLevel() const;
	void SetLevel(const int32 InLevel);
	float GetHealth() const;
	void SetHealth(const float InHealth);
	float GetMaxHealth() const;
	void SetMaxHealth(const float InMaxHealth);
	UFUNCTION(BlueprintCallable, BlueprintPure, FieldNotify)
	float GetHealthPercent() const;
	float GetMana() const;
	void SetMana(const float InMana);
	float GetMaxMana() const;
	void SetMaxMana(const float InMaxMana);
	UFUNCTION(BlueprintCallable, BlueprintPure, FieldNotify)
	float GetManaPercent() const;
	int32 GetXP() const;
	void SetXP(const int32 InXP);
	int32 GetXPMax() const;
	void SetXPMax(const int32 InXPMax);
	UFUNCTION(BlueprintCallable, BlueprintPure, FieldNotify)
	float GetXPPercent() const;
	UTexture2D* GetPortraitIcon() const;
	void SetPortraitIcon(UTexture2D* InPortraitIcon);
	UFUNCTION(BlueprintCallable, BlueprintPure, FieldNotify)
	FSlateBrush GetPortraitBrush() const;
	bool GetIsActive() const;
	void SetIsActive(const bool InActive);
	bool GetIsDead() const;
	void SetIsDead(const bool InIsDead);

	void InitializeDependencies(const AElectricCastlePlayerState* PlayerState);

protected:
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	int32 PlayerIndex = 0;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	int32 Level = 0;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	float Health = 0;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	float MaxHealth = 0;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	float Mana = 0;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	float MaxMana = 0;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	int32 XP = 0;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	int32 XPMax = 0;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	TObjectPtr<UTexture2D> PortraitIcon;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	bool IsActive = false;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	bool IsDead = false;

private:
	UFUNCTION()
	void OnPlayerXPChange(const FIntAttributeChangedPayload& Payload);
	UFUNCTION()
	void OnPlayerLevelInitialized(const FIntAttributeChangedPayload& Payload);
	UFUNCTION()
	void OnPlayerLevelChange(const FIntAttributeChangedPayload& Payload);
	UFUNCTION()
	void OnFormChange(const FPlayerFormConfigRow& Payload);
};
