// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverlayWidget.generated.h"

class UMVVM_PlayerState;
/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UOverlayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<UMVVM_PlayerState*> GetPlayerStateViewModels() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Show();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Hide();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void BindViewModels(const TArray<UMVVM_PlayerState*>& ViewModels);
};
