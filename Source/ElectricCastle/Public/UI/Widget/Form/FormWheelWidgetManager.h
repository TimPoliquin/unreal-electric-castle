// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UI/ViewModel/Form/MVVM_PlayerForms.h"
#include "FormWheelWidgetManager.generated.h"

class UMVVM_PlayerForms;
class UFormWheelWidget;
class UNamedSlot;
/**
 * 
 */
UCLASS(Abstract)
class ELECTRICCASTLE_API UFormWheelWidgetManager : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void BindPlayerFormsViewModel(UMVVM_PlayerForms* ViewModel);
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	UPanelWidget* GetFormWheelPanel();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties")
	TSubclassOf<UFormWheelWidget> FormWheelWidgetClass;

private:
	UFUNCTION()
	void HandlePlayerFormVisibilityChange(const FPlayerFormsVisibilityChangePayload& Payload);
	void ShowFormWheelWidget(UMVVM_PlayerForms* ViewModel);
	void HideFormWheelWidget(UMVVM_PlayerForms* ViewModel);
	UFormWheelWidget* CreateFormWheelWidget(UMVVM_PlayerForms* PlayerFormsViewModel);

	UPROPERTY()
	TMap<int32, UFormWheelWidget*> FormWheelWidgetMap;
};
