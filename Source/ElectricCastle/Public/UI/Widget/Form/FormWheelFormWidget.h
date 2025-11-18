// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "FormWheelFormWidget.generated.h"

class UMVVM_PlayerForm;
/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UFormWheelFormWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FGameplayTag GetFormTag() const;
	UFUNCTION(BlueprintNativeEvent)
	void BindViewModel(UMVVM_PlayerForm* InViewModel);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Hover();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Unhover();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Select();
	bool IsFormSelected() const;

private:
	UPROPERTY()
	TObjectPtr<UMVVM_PlayerForm> PlayerFormViewModel;
};
