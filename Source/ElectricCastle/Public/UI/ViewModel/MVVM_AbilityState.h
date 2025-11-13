// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MVVMViewModelBase.h"
#include "AbilitySystem/AbilityChangeDelegates.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "MVVM_AbilityState.generated.h"

class UElectricCastleAbilitySystemComponent;
class UAbilitySystemComponent;
class FOnAbilityChangedSignature;
/**
 * 
 */
UCLASS(Abstract)
class ELECTRICCASTLE_API UMVVM_AbilityState : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, FieldNotify)
	bool IsActive() const;
	FGameplayTag GetAbilityTag() const;
	void SetAbilityTag(const FGameplayTag& InAbilityTag);
	UTexture2D* GetAbilityIcon() const;
	void SetAbilityIcon(UTexture2D* InAbilityIcon);
	UFUNCTION(BlueprintCallable, BlueprintPure, FieldNotify)
	FSlateBrush GetAbilityIconBrush() const;
	UMaterialInterface* GetAbilityBackground() const;
	void SetAbilityBackground(UMaterialInterface* InAbilityBackground);
	UFUNCTION(BlueprintCallable, BlueprintPure, FieldNotify)
	FSlateBrush GetAbilityBackgroundBrush() const;
	FGameplayTag GetInputTag() const;
	void SetInputTag(const FGameplayTag& InInputTag);
	float GetCooldown_RemainingTime() const;
	void SetCooldown_RemainingTime(const float InCooldown_RemainingTime);
	float GetCooldown_TotalTime() const;
	void SetCooldown_TotalTime(const float InCooldown_TotalTime);
	FGameplayTag GetCooldownTag() const;
	void SetCooldownTag(const FGameplayTag& InCooldownTag);
	bool GetIsCoolingDown() const;
	void SetIsCoolingDown(const bool bIsCoolingDown);
	bool GetIsBlocked() const;
	void SetIsBlocked(const bool bIsBlocked);
	UFUNCTION(BlueprintCallable, BlueprintPure, FieldNotify)
	bool GetIsAbilityAvailable() const;
	FElectricCastleAbilityInfo GetAbilityInfo() const;
	void SetAbilityInfo(const FElectricCastleAbilityInfo& InAbilityInfo);

	void InitializeDependencies(APlayerState* PlayerState);
	void UpdateCooldown(float DeltaTime);

protected:
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	FGameplayTag AbilityTag = FGameplayTag::EmptyTag;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	TObjectPtr<UTexture2D> AbilityIcon;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	TObjectPtr<UMaterialInterface> AbilityBackground;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	FGameplayTag InputTag = FGameplayTag::EmptyTag;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	float Cooldown_RemainingTime = 0;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	float Cooldown_TotalTime = 0;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	FGameplayTag CooldownTag = FGameplayTag::EmptyTag;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	bool IsCoolingDown = false;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	bool IsBlocked = false;

private:
	void StartCooldown(float InDuration);
	void StartCooldown(const FActiveGameplayEffectHandle& InCooldownEffectHandle);

	void ClearCooldown();

	UFUNCTION()
	void OnPlayerAbilityAdded(const FOnAbilityChangedPayload& Payload);
	UFUNCTION()
	void OnPlayerAbilityRemoved(const FOnAbilityChangedPayload& Payload);
	FActiveGameplayEffectHandle FindActiveGameplayEffectWithCooldownTag(FGameplayTag InCooldownTag) const;
	UFUNCTION()
	void OnCooldownTagChanged(
		const FGameplayTag InCooldownTag,
		const int NewCount
	);
	UPROPERTY()
	TObjectPtr<UElectricCastleAbilitySystemComponent> AbilitySystemComponent;
	FElectricCastleAbilityInfo AbilityInfo;
	FDelegateHandle CooldownDelegateHandle;
};
