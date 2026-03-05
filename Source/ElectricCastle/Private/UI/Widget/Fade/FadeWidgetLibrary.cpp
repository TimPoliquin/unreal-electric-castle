// Copyright Alien Shores


#include "UI/Widget/Fade/FadeWidgetLibrary.h"

#include "Components/Widget.h"
#include "Engine/World.h"
#include "UI/Widget/Fade/FadeWidgetLatentAction.h"

void UFadeWidgetLibrary::FadeWidget(
	UObject* WorldContextObject,
	UWidget* Widget,
	const float TargetOpacity,
	const float Duration,
	const FLatentActionInfo LatentInfo)
{
	if (!WorldContextObject || !Widget)
	{
		return;
	}

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		return;
	}

	FLatentActionManager& Manager = World->GetLatentActionManager();

	if (Manager.FindExistingAction<FFadeWidgetLatentAction>(LatentInfo.CallbackTarget, LatentInfo.UUID))
	{
		Manager.RemoveActionsForObject(LatentInfo.CallbackTarget);
	}

	const float StartOpacity = Widget->GetRenderOpacity();

	Manager.AddNewAction(
		LatentInfo.CallbackTarget,
		LatentInfo.UUID,
		new FFadeWidgetLatentAction(
			Widget,
			StartOpacity,
			TargetOpacity,
			Duration,
			LatentInfo));
}
