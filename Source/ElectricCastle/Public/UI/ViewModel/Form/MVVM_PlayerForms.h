// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Player/InputEvents.h"
#include "Player/Form/FormConfigTypes.h"
#include "Player/Form/PlayerFormDelegates.h"
#include "Player/Form/PlayerFormPrimaryAsset.h"
#include "Player/SelectionWheel/SelectionWheelManagerActorInterface.h"
#include "UI/Widget/Form/FormWheelFormWidget.h"
#include "MVVM_PlayerForms.generated.h"

class UMVVM_PlayerForms;
struct FGameplayTag;
struct FPlayerFormConfigRow;
class UMVVM_PlayerForm;
class AElectricCastlePlayerState;

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FPlayerFormsVisibilityChangePayload
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMVVM_PlayerForms> ViewModel;
	UPROPERTY(BlueprintReadOnly)
	bool bIsVisible = false;

	bool IsValid() const
	{
		return ViewModel != nullptr;
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerFormsVisiblityChangeSignature, const FPlayerFormsVisibilityChangePayload&, Payload);

/**
 * 
 */
UCLASS(Abstract)
class ELECTRICCASTLE_API UMVVM_PlayerForms : public UMVVMViewModelBase, public ISelectionWheelManagerActorInterface
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
	UFUNCTION(BlueprintCallable, BlueprintPure)
	AElectricCastlePlayerController* GetPlayerController() const;
	void ChangeForm(const FGameplayTag& FormTag);;
	UPROPERTY(BlueprintAssignable)
	FOnPlayerFormsVisiblityChangeSignature OnVisibilityChange;
	UPROPERTY(BlueprintAssignable)
	FOnPlayerAvailableFormsChangedSignature OnAvailableFormsChangedDelegate;

	/** Start ISelectionWheelManagerActorInterface **/
	virtual USelectionWheelManagerComponent* GetSelectionWheelManagerComponent_Implementation() const override;
	/** End ISelectionWheelManagerActorInterface **/

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
		const AElectricCastlePlayerState* InPlayerState, const UPlayerFormPrimaryAsset* FormAsset
	);

	UFUNCTION()
	void OnFormWheelVisibilityChange(const FOnPlayerFormWheelVisibilityChangePayload& Payload);
	UFUNCTION()
	void OnAvailableFormsChanged(const FOnPlayerAvailableFormsChangedPayload& Payload);
};
