// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "WidgetVisibilityEvents.h"
#include "Blueprint/UserWidget.h"
#include "AuraFishingMessageWidget.generated.h"

class UInputAction;

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UAuraFishingMessageWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category="Event")
	FWidgetEventSignature OnContinue;
	UPROPERTY(BlueprintAssignable, Category="Event")
	FWidgetEventSignature OnCancel;
	UPROPERTY(BlueprintAssignable, Category="Event")
	FWidgetEventSignature OnShown;
	UPROPERTY(BlueprintAssignable, Category="Event")
	FWidgetEventSignature OnHidden;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Show();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Hide();

protected:
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Continue();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Cancel();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Shown();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Hidden();

	UFUNCTION(BlueprintCallable)
	bool IsKeyBoundToInputAction(const FKeyEvent& Input, const UInputAction* InputAction) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	FKey CancelKey = EKeys::Escape;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	FKey ContinueKey = EKeys::Enter;

private:
	bool bIsShowing = false;
	bool bAllowInput = false;
};
