// Copyright Alien Shores


#include "UI/Widget/Fade/FadeWidgetLibrary.h"

#include "Components/Widget.h"
#include "Engine/World.h"
#include "UI/Widget/Fade/FadeWidgetLatentAction.h"

int32 UFadeWidgetLibrary::FadeWidget(
	UObject* WorldContextObject,
	UWidget* Widget,
	const float TargetOpacity,
	const float Duration,
	const FLatentActionInfo LatentInfo
)
{
	if (!WorldContextObject || !Widget)
	{
		return -1;
	}

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		return -1;
	}

	FLatentActionManager& Manager = World->GetLatentActionManager();

	Manager.RemoveActionsForObject(LatentInfo.CallbackTarget);

	const float StartOpacity = Widget->GetRenderOpacity();

	Manager.AddNewAction(
		LatentInfo.CallbackTarget,
		LatentInfo.UUID,
		new FFadeWidgetLatentAction(
			Widget,
			StartOpacity,
			TargetOpacity,
			Duration,
			LatentInfo
		)
	);
	return LatentInfo.UUID;
}
