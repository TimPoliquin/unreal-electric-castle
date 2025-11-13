// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerAbilityStateWidget.generated.h"

class UMVVM_AbilityState;
/**
 * 
 */
UCLASS(Abstract)
class ELECTRICCASTLE_API UPlayerAbilityStateWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void BindViewModel(UMVVM_AbilityState* InViewModel);
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UPROPERTY()
	TObjectPtr<UMVVM_AbilityState> ViewModel;
};
