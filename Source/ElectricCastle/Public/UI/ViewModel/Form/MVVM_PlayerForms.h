// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Player/InputEvents.h"
#include "Player/Form/FormConfigTypes.h"
#include "Player/Form/PlayerFormDelegates.h"
#include "MVVM_PlayerForms.generated.h"

struct FGameplayTag;
struct FPlayerFormConfigRow;
class UMVVM_PlayerForm;
class AElectricCastlePlayerState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerFormsVisiblityChangeSignature, const bool, bIsVisible);

/**
 * 
 */
UCLASS(Abstract)
class ELECTRICCASTLE_API UMVVM_PlayerForms : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	int32 GetPlayerIndex() const { return PlayerIndex; };
	void SetPlayerIndex(const int32 InPlayerIndex) { PlayerIndex = InPlayerIndex; };
	void InitializeDependencies(AElectricCastlePlayerState* PlayerState);
	FName CreateWidgetName(const FString& InWidgetName) const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UMVVM_PlayerForm* GetPlayerFormViewModelById(const EPlayerForm Form) const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UMVVM_PlayerForm* GetPlayerFormViewModelByTag(const FGameplayTag& FormTag) const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<UMVVM_PlayerForm*> GetAvailablePlayerFormViewModels() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<UMVVM_PlayerForm*> GetPlayerFormViewModels() const;
	UPROPERTY(BlueprintAssignable)
	FOnPlayerFormsVisiblityChangeSignature OnVisibilityChange;
	UPROPERTY(BlueprintAssignable)
	FOnPlayerAvailableFormsChangedSignature OnAvailableFormsChangedDelegate;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties")
	TSubclassOf<UMVVM_PlayerForm> PlayerFormViewModelClass;
	UPROPERTY()
	TMap<EPlayerForm, TObjectPtr<UMVVM_PlayerForm>> PlayerFormViewModels;

private:
	int32 PlayerIndex = 0;

	void CreatePlayerFormViewModels(AElectricCastlePlayerState* PlayerState);
	UMVVM_PlayerForm* CreatePlayerFormViewModel(
		AElectricCastlePlayerState* PlayerState,
		const FPlayerFormConfigRow& FormConfigRow
	);

	UFUNCTION()
	void OnFormWheelVisibilityChange(const FOnPlayerFormWheelVisibilityChangePayload& Payload);
	UFUNCTION()
	void OnAvailableFormsChanged(const FOnPlayerAvailableFormsChangedPayload& Payload);
};
