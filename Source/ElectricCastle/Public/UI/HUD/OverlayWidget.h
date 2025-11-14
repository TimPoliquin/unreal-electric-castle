// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Player/ElectricCastlePlayerState.h"
#include "Player/InputEvents.h"
#include "OverlayWidget.generated.h"

class UFormWheelWidget;
class AElectricCastlePlayerState;
class UMVVM_PlayerForms;
class UMVVM_PlayerAbilityStates;
class UMVVM_PlayerState;
/**
 * 
 */
UCLASS(Abstract)
class ELECTRICCASTLE_API UOverlayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<UMVVM_PlayerState*> GetPlayerStateViewModels() const;
	UFUNCTION(BlueprintCallable)
	void InitializeDependencies(AElectricCastlePlayerState* PlayerState) const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Show();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Hide();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ShowFormWheel();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void HideFormWheel();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void BindViewModels(
		const TArray<UMVVM_PlayerState*>& ViewModels,
		const TArray<UMVVM_PlayerAbilityStates*>& AbilityViewModels,
		const TArray<UMVVM_PlayerForms*>& FormsViewModels
	);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties")
	TSubclassOf<UFormWheelWidget> FormWheelWidgetClass;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Components")
	TArray<TObjectPtr<UFormWheelWidget>> FormWheelWidgets;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void CreateFormWheelWidgets(TArray<UMVVM_PlayerForms*>& PlayerFormsViewModels, UPanelWidget* ParentWidget);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	UFormWheelWidget* CreateFormWheelWidget(UMVVM_PlayerForms* PlayerFormsViewModel);
	UFUNCTION(BlueprintCallable)
	UFormWheelWidget* GetFormWheelWidgetByPlayerIndex(const int32 PlayerIndex) const;

private:
	UFUNCTION()
	void OnFormWheelVisibilityChange(const FOnPlayerFormWheelVisibilityChangePayload& Payload);
};
