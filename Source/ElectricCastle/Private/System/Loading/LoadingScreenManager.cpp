// Copyright Alien Shores


#include "System/Loading/LoadingScreenManager.h"

#include "Engine/Engine.h"
#include "System/Loading/SLoadingScreenWidget.h"

#include "Engine/Texture2D.h"
#include "Slate/DeferredCleanupSlateBrush.h"

TSharedPtr<FDeferredCleanupSlateBrush> FLoadingScreenManager::BackgroundBrush = nullptr;

void FLoadingScreenManager::Initialize()
{
	UTexture2D* BGTexture = LoadObject<UTexture2D>(
		nullptr,
		TEXT("/Game/Blueprints/UI/LoadingScreen/LoadingBackground")
	);

	if (BGTexture)
	{
		BackgroundBrush = FDeferredCleanupSlateBrush::CreateBrush(
			BGTexture,
			FVector2D(1920.f, 1080.f)
		);
	}

	FCoreUObjectDelegates::PreLoadMap.AddStatic(&FLoadingScreenManager::OnPreLoadMap);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddStatic(&FLoadingScreenManager::OnPostLoadMap);
}

void FLoadingScreenManager::Shutdown()
{
	FCoreUObjectDelegates::PreLoadMap.RemoveAll(nullptr);
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(nullptr);
}

void FLoadingScreenManager::OnPreLoadMap(const FString& MapName)
{
	if (IsMoviePlayerEnabled())
	{
		FLoadingScreenAttributes Attributes = CreateLoadingScreenAttributes();
		GetMoviePlayer()->SetupLoadingScreen(Attributes);
	}
}

void FLoadingScreenManager::OnPostLoadMap(UWorld* LoadedWorld)
{
	// The MoviePlayer stops automatically, but you can trigger
	// additional fade-out logic or analytics here.
	if (!LoadedWorld)
	{
		return;
	}

	// Schedule the fade on the next frame so the player controller is ready
	LoadedWorld->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([LoadedWorld]()
	{
		if (const APlayerController* PlayerController = LoadedWorld->GetFirstPlayerController(); PlayerController && PlayerController->PlayerCameraManager)
		{
			// Fade from black over 1 second
			PlayerController->PlayerCameraManager->StartCameraFade(
				1.f, // From (fully black)
				0.f, // To (fully visible)
				1.0f, // Duration in seconds
				FLinearColor::Black,
				false, // Hold when finished
				true // Fade audio too
			);
		}
	}));
}

FLoadingScreenAttributes FLoadingScreenManager::CreateLoadingScreenAttributes()
{
	FLoadingScreenConfig Config;

	Config.TitleText = FText::FromString(TEXT("Loading..."));

	Config.Tips = {
	};

	Config.TipRotationInterval = 5.0f;

	// To use a background image, create an FSlateDynamicImageBrush from a
	// texture that is already loaded into memory. Using a texture from a
	// plugin or a hardcoded engine asset is safest here since the asset
	// system is busy during loading. See note below.
	Config.BackgroundBrush = BackgroundBrush.IsValid() ? BackgroundBrush->GetSlateBrush() : nullptr;

	FLoadingScreenAttributes Attributes;
	Attributes.WidgetLoadingScreen = SNew(SLoadingScreenWidget).Config(Config);
	Attributes.bAutoCompleteWhenLoadingCompletes = true;
	Attributes.bMoviesAreSkippable = false;
	Attributes.MinimumLoadingScreenDisplayTime = 2.0f;

	return Attributes;
}
