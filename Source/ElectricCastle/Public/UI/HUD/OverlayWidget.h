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
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Show();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Hide();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ShowFormWheel();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void HideFormWheel();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ShowMajorNotification(UUserWidget* NotificationWidget, UNamedSlot* NotificationSlot);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void BindPlayerStateViewModel(UMVVM_PlayerState* ViewModel);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void BindPlayerAbilityStatesViewModel(UMVVM_PlayerAbilityStates* ViewModel);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void BindPlayerFormsViewModel(UMVVM_PlayerForms* ViewModel);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties")
	TSubclassOf<UFormWheelWidget> FormWheelWidgetClass;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Components")
	TArray<TObjectPtr<UFormWheelWidget>> FormWheelWidgets;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	UFormWheelWidget* CreateFormWheelWidget(UPanelWidget* ParentWidget, UMVVM_PlayerForms* PlayerFormsViewModel);
	UFUNCTION(BlueprintCallable)
	UFormWheelWidget* GetFormWheelWidgetByPlayerIndex(const int32 PlayerIndex) const;

private:
	UFUNCTION()
	void OnFormWheelVisibilityChange(const FOnPlayerFormWheelVisibilityChangePayload& Payload);
};
