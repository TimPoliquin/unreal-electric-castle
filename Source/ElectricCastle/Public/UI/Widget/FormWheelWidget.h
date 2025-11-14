// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FormWheelWidget.generated.h"

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
	void Show();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Hide();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void BindViewModel(UMVVM_PlayerForms* InPlayerFormsViewModel);

private:
	TObjectPtr<UMVVM_PlayerForms> PlayerFormsViewModel;
};
