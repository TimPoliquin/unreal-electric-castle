// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WidgetFunctionLibrary.generated.h"

class UInputAction;
class UWidget;
class UProgressBar;
class UOverlaySlot;
class USizeBox;
/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UWidgetFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "WidgetFunctionLibrary")
	static void SetBoxSizeOverride(USizeBox* SizeBox, float Height, float Width);

	UFUNCTION(BlueprintCallable, Category = "WidgetFunctionLibrary")
	static void SetWidgetPadding(UWidget* Widget, float Padding);

	UFUNCTION(
		BlueprintCallable,
		Category = "WidgetFunctionLibrary|ProgressBar",
		meta = (DefaultColor = "0.0, 0.0, 0.0, 0.0")
	)
	static void SetProgressBarImage(
		UProgressBar* ProgressBar,
		const FSlateBrush& FillImage
	);

	UFUNCTION(
		BlueprintCallable,
		Category = "WidgetFunctionLibrary|ProgressBar"
	)
	static void InterpolateProgressBarValue(
		UProgressBar* ProgressBar,
		float TargetValue,
		float DeltaTime,
		float InterpSpeed
	);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="WidgetFunctionLibrary|Controller")
	static APlayerController* GetPlayerController(const AActor* Player);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="WidgetFunctionLibrary|Input")
	static bool IsInputActionBoundToKey(
		const UObject* WorldContextObject,
		int32 PlayerIndex,
		const UInputAction* InputAction,
		const FKey& Key
	);
};
