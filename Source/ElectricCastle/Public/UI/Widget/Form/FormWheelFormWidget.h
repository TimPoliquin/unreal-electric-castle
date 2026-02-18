// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "UI/Widget/HoverableWidget.h"
#include "FormWheelFormWidget.generated.h"

class UMVVM_PlayerForm;
/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UFormWheelFormWidget : public UUserWidget, public IHoverableWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FGameplayTag GetFormTag() const;
	UFUNCTION(BlueprintNativeEvent)
	void BindViewModel(UMVVM_PlayerForm* InViewModel);

	bool IsFormSelected() const;

	/** Start IHoverableWidget **/
	virtual void Hover_Implementation() override;
	virtual void Unhover_Implementation() override;
	/** End IHoverableWidget **/

private:
	UPROPERTY()
	TObjectPtr<UMVVM_PlayerForm> PlayerFormViewModel;
};
