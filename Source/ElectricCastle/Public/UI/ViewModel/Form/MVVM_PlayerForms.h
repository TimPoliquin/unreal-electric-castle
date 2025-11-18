// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Player/InputEvents.h"
#include "Player/Form/FormConfigTypes.h"
#include "Player/Form/PlayerFormDelegates.h"
#include "UI/Widget/Form/FormWheelFormWidget.h"
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
	int32 GetPlayerIndex() const { return PlayerIndex; }
	void SetPlayerIndex(const int32 InPlayerIndex) { PlayerIndex = InPlayerIndex; };
	void InitializeDependencies(AElectricCastlePlayerState* InPlayerState);
	FName CreateWidgetName(const FString& InWidgetName) const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UMVVM_PlayerForm* GetPlayerFormViewModelById(const EPlayerForm Form) const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UMVVM_PlayerForm* GetPlayerFormViewModelByTag(const FGameplayTag& FormTag) const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<UMVVM_PlayerForm*> GetAvailablePlayerFormViewModels() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<UMVVM_PlayerForm*> GetPlayerFormViewModels() const;
	void ChangeForm(const FGameplayTag& FormTag);;
	UPROPERTY(BlueprintAssignable)
	FOnPlayerFormsVisiblityChangeSignature OnVisibilityChange;
	UPROPERTY(BlueprintAssignable)
	FOnPlayerAvailableFormsChangedSignature OnAvailableFormsChangedDelegate;
	UPROPERTY(BlueprintAssignable)
	FOnPlayerFormWheelHighlightChangedSignature OnFormWheelHighlightChangeDelegate;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties")
	TSubclassOf<UMVVM_PlayerForm> PlayerFormViewModelClass;
	UPROPERTY()
	TMap<EPlayerForm, TObjectPtr<UMVVM_PlayerForm>> PlayerFormViewModels;

private:
	int32 PlayerIndex = 0;
	UPROPERTY()
	TObjectPtr<APlayerState> PlayerState;

	void CreatePlayerFormViewModels(AElectricCastlePlayerState* InPlayerState);
	UMVVM_PlayerForm* CreatePlayerFormViewModel(
		AElectricCastlePlayerState* InPlayerState,
		const FPlayerFormConfigRow& FormConfigRow
	);

	UFUNCTION()
	void OnFormWheelVisibilityChange(const FOnPlayerFormWheelVisibilityChangePayload& Payload);
	UFUNCTION()
	void OnAvailableFormsChanged(const FOnPlayerAvailableFormsChangedPayload& Payload);
	UFUNCTION()
	void OnFormWheelHighlightChange(const FOnPlayerFormWheelHighlightChangedPayload& Payload);
};
