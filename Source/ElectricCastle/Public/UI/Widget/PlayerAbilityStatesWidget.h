// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerAbilityStatesWidget.generated.h"

class UMVVM_PlayerAbilityStates;
/**
 * 
 */
UCLASS(Abstract)
class ELECTRICCASTLE_API UPlayerAbilityStatesWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void BindViewModel(UMVVM_PlayerAbilityStates* ViewModel);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Show();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Hide();
};
