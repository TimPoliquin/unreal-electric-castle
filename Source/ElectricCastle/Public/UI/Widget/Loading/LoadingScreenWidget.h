// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoadingScreenWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class ELECTRICCASTLE_API ULoadingScreenWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Show(const bool bImmediate = false);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Hide(const bool bImmediate = false);
};
