// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "FishTypes.h"
#include "GameplayAbilitySpecHandle.h"
#include "Components/ActorComponent.h"
#include "Item/Equipment/FishingBobActor.h"
#include "Item/ItemTypes.h"
#include "Player/Equipment/EquipmentTypes.h"
#include "FishingComponent.generated.h"


struct FEquipmentDelegatePayload;
class UPlayerEquipmentComponent;
struct FGameplayAbilitySpecHandle;
class AFishingBobActor;
class AFishingRodActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFishingComponentEquipAnimationSignature);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFishingComponentCastAnimationSignature);

struct FAfterFishingRestore
{
	bool bSet = false;
	EEquipmentUseMode UseMode = EEquipmentUseMode::None;
	FGameplayTag WeaponType = FGameplayTag::EmptyTag;
	FGameplayTag ToolType = FGameplayTag::EmptyTag;

	void Reset()
	{
		bSet = false;
		UseMode = EEquipmentUseMode::None;
		WeaponType = FGameplayTag::EmptyTag;
		ToolType = FGameplayTag::EmptyTag;
	}
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API UFishingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFishingComponent();
	void SetPlayerEquipmentComponent(UPlayerEquipmentComponent* InPlayerEquipmentComponent);
	UFUNCTION(BlueprintCallable)
	virtual void SetupForFishing(const FVector& InFishingDestination);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual bool HasFishingRod() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual bool HasFishingRodEquipped() const;
	UFUNCTION(BlueprintCallable)
	virtual void EquipFishingRod();
	UFUNCTION(BlueprintCallable)
	virtual void CastFishingRod();
	virtual void FishStateChanged(const EFishState& FishState);
	virtual void Reel();
	virtual FOnFishingStateChangedSignature& GetOnFishingStateChangedDelegate();
	virtual void PrepareForContinue();
	UFUNCTION(BlueprintCallable)
	virtual void EndFishing();
	UFUNCTION(BlueprintCallable)
	void ReleaseCast();
	virtual bool IsFishing() const;
	UFUNCTION(BlueprintCallable)
	EFishingState GetFishingState() const;
	UFUNCTION(BlueprintCallable)
	virtual float GetRarityMultiplier(const FGameplayTag& Rarity) const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual FGameplayTagContainer GetFishingTags() const;

	UPROPERTY(BlueprintAssignable)
	FOnFishingStateChangedSignature OnFishingStateChangedDelegate;
	UPROPERTY(BlueprintAssignable)
	FOnFishingComponentCastAnimationSignature OnFishingComponentCastAnimationDelegate;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EFishingState FishingState = EFishingState::None;

private:
	void SetFishingState(EFishingState InFishingState);
	void OnFishingRodEquipped();
	UFUNCTION()
	void OnFishingBobStateChanged(EFishingBobState FishingBobState);
	UFUNCTION()
	void OnInventoryEquipAnimationComplete(const FEquipmentDelegatePayload& Payload);
	UPROPERTY()
	TObjectPtr<UPlayerEquipmentComponent> PlayerEquipmentComponent;
	UPROPERTY()
	TObjectPtr<AFishingRodActor> FishingRod;
	UPROPERTY()
	TObjectPtr<AFishingBobActor> FishingBob;
	UPROPERTY()
	FVector FishingDestination = FVector::ZeroVector;
	FAfterFishingRestore FishingRestore;
	UPROPERTY()
	FGameplayAbilitySpecHandle ActiveReelAbilityHandle;
};
