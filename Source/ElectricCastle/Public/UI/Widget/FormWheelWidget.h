// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Player/Form/PlayerFormDelegates.h"
#include "FormWheelWidget.generated.h"

class UFormWheelFormWidget;
class UMVVM_PlayerForms;
/**
 * 
 */
UCLASS(Abstract)
class ELECTRICCASTLE_API UFormWheelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetPlayerIndex() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Show(const bool bAnimate = true);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Hide(const bool bAnimate = true);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void BindViewModel(UMVVM_PlayerForms* InPlayerFormsViewModel);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties")
	TSubclassOf<UFormWheelFormWidget> FormWidgetClass;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	UPanelWidget* GetFormsContainer() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void CreateFormWidgets(UMVVM_PlayerForms* PlayerForms);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetSelectedIndex() const;

private:
	UPROPERTY()
	TObjectPtr<UMVVM_PlayerForms> PlayerFormsViewModel;
	UPROPERTY()
	TArray<TObjectPtr<UFormWheelFormWidget>> FormWidgets;
	UFUNCTION()
	void OnFormWheelVisibilityChange(const bool bIsVisible);
	UFUNCTION()
	void OnAvailableFormsChanged(const FOnPlayerAvailableFormsChangedPayload& Payload);
};
