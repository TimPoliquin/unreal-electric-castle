// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Player/Form/PlayerFormDelegates.h"
#include "Player/SelectionWheel/SelectionWheelSubscriberInterface.h"
#include "FormWheelWidget.generated.h"

class URadialLayout;
class URadialLayoutCursor;
class UFormWheelFormWidget;
class UMVVM_PlayerForms;
/**
 * 
 */
UCLASS(Abstract)
class ELECTRICCASTLE_API UFormWheelWidget : public UUserWidget, public ISelectionWheelSubscriberInterface
{
	GENERATED_BODY()

public:
	virtual void NativeDestruct() override;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetPlayerIndex() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Show(const bool bAnimate = true);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Hide(const bool bAnimate = true);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void BindViewModel(UMVVM_PlayerForms* InPlayerFormsViewModel);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void CommitSelection();

	/** Start ISelectionWheelSubscriberInterface **/
	virtual void OnSelectionWheelAngleChange_Implementation(float Value) override;
	virtual void OnSelectionWheelConfirm_Implementation() override;
	/** End ISelectionWheelSubscriberInterface **/

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties")
	TSubclassOf<UFormWheelFormWidget> FormWidgetClass;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	URadialLayout* GetFormsContainer() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	URadialLayoutCursor* GetCursorWidget() const;
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
	void OnAvailableFormsChanged(const FOnPlayerAvailableFormsChangedPayload& Payload);
};
