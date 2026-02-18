// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverlayWidget.generated.h"

class UFormWheelWidgetManager;
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
	void ShowMajorNotification(UUserWidget* NotificationWidget, UNamedSlot* NotificationSlot);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void BindPlayerStateViewModel(UMVVM_PlayerState* ViewModel);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void BindPlayerAbilityStatesViewModel(UMVVM_PlayerAbilityStates* ViewModel);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void BindPlayerFormsViewModel(UMVVM_PlayerForms* ViewModel);
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	UFormWheelWidgetManager* GetFormWheelWidgetManager();
};
