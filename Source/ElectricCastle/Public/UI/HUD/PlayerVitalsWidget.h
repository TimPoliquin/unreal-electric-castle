// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerVitalsWidget.generated.h"

class UMVVM_PlayerState;
/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UPlayerVitalsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void BindViewModels(const UMVVM_PlayerState* ViewModel);
};
