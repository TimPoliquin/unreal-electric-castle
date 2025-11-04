// Copyright Alien Shores


#include "UI/Library/WidgetFunctionLibrary.h"

#include "EnhancedInputSubsystems.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/OverlaySlot.h"
#include "Components/ProgressBar.h"
#include "Components/SizeBox.h"
#include "Brushes/SlateColorBrush.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Panel/PanelInterface.h"

void UWidgetFunctionLibrary::SetBoxSizeOverride(USizeBox* SizeBox, float Height, float Width)
{
	SizeBox->SetWidthOverride(Width);
	SizeBox->SetHeightOverride(Height);
}

void UWidgetFunctionLibrary::SetWidgetPadding(UWidget* Widget, float Padding)
{
	if (!Widget)
	{
		return;
	}
	if (UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(Widget->Slot))
	{
		const FMargin Margin(Padding);
		OverlaySlot->SetPadding(Margin);
	}
}

void UWidgetFunctionLibrary::SetProgressBarImage(
	UProgressBar* ProgressBar,
	const FSlateBrush& FillImage
)
{
	const FSlateColor SlateColor(FColor::Transparent);
	const FSlateColorBrush BackgroundImage(SlateColor);
	FProgressBarStyle Style;
	Style.SetBackgroundImage(BackgroundImage);
	Style.SetFillImage(FillImage);
	ProgressBar->SetWidgetStyle(Style);
	ProgressBar->SetFillColorAndOpacity(FColor::White);
}

void UWidgetFunctionLibrary::InterpolateProgressBarValue(
	UProgressBar* ProgressBar,
	const float TargetValue,
	const float DeltaTime,
	const float InterpSpeed
)
{
	if (ProgressBar->GetVisibility() == ESlateVisibility::Hidden)
	{
		return;
	}
	if (TargetValue == ProgressBar->GetPercent())
	{
		return;
	}
	ProgressBar->SetPercent(FMath::FInterpTo(ProgressBar->GetPercent(), TargetValue, DeltaTime, InterpSpeed));
}

APlayerController* UWidgetFunctionLibrary::GetPlayerController(const AActor* Player)
{
	if (const APawn* Pawn = Cast<APawn>(Player))
	{
		return Cast<APlayerController>(Pawn->GetController());
	}
	return nullptr;
}

bool UWidgetFunctionLibrary::IsInputActionBoundToKey(
	const UObject* WorldContextObject,
	const int32 PlayerIndex,
	const UInputAction* InputAction,
	const FKey& Key
)
{
	const ULocalPlayer* LocalPlayer = IsValid(WorldContextObject)
		                                  ? GEngine->GetGamePlayer(WorldContextObject->GetWorld(), PlayerIndex)
		                                  : nullptr;
	const UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer
		                                                           ? LocalPlayer->GetSubsystem<
			                                                           UEnhancedInputLocalPlayerSubsystem>()
		                                                           : nullptr;
	const TArray<FKey>& Keys = InputSubsystem
		                           ? InputSubsystem->QueryKeysMappedToAction(InputAction)
		                           : TArray<FKey>();
	return Keys.Contains(Key);
}
