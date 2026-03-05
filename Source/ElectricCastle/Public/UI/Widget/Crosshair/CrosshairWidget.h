// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/ViewModel/MVVM_Crosshair.h"
#include "CrosshairWidget.generated.h"

class UImage;
class UWidgetFadeController;
/**
 * 
 */
UCLASS(Abstract)
class ELECTRICCASTLE_API UCrosshairWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InitializeDependencies(UMVVM_Crosshair* CrosshairViewModel);

protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void TransitionTo_Hidden();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void TransitionTo_Subtle();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void TransitionTo_Visible();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void TransitionTo_StyleTargeting();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void TransitionTo_StyleDefault();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetCrosshairState(ECrosshairState InCrosshairState);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetCrosshairStyle(ECrosshairStyle InCrosshairStyle);

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	ECrosshairState CrosshairState = ECrosshairState::Active;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	ECrosshairStyle CrosshairStyle = ECrosshairStyle::Targeting;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components", meta=(BindWidget))
	TObjectPtr<UImage> CrosshairImage;

private:
	void HandleCrosshairStateChanged(UObject* ViewModel, UE::FieldNotification::FFieldId FieldId);
	void HandleCrosshairStyleChanged(UObject* Object, UE::FieldNotification::FFieldId FieldId);
};
