// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "MoviePlayer.h"
#include "Brushes/SlateImageBrush.h"

class FDeferredCleanupSlateBrush;

class FLoadingScreenManager
{
public:
	static void Initialize();
	static void Shutdown();

private:
	static void OnPreLoadMap(const FString& MapName);
	static void OnPostLoadMap(UWorld* LoadedWorld);
	static FLoadingScreenAttributes CreateLoadingScreenAttributes();

	static TSharedPtr<FDeferredCleanupSlateBrush> BackgroundBrush;
};
