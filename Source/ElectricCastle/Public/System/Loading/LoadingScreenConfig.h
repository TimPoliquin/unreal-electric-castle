#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateBrush.h"

// Plain struct — no UObject, safe to pass to the render thread
struct FLoadingScreenConfig
{
	// Background image — must be a pre-loaded brush, not a lazy-loaded asset,
	// since the asset system is busy loading the next level
	const FSlateBrush* BackgroundBrush = nullptr;

	// Large title text shown prominently on screen
	FText TitleText = FText::FromString(TEXT("Loading..."));

	// Rotating tips shown at the bottom — leave empty to hide the tip line
	TArray<FText> Tips;

	// How many seconds each tip is shown before rotating
	float TipRotationInterval = 5.0f;
};
