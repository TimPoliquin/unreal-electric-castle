// Copyright Alien Shores


#include "UI/Widget/Fade/FadeWidgetLatentAction.h"
// FadeWidgetLatentAction.cpp
#include "Blueprint/UserWidget.h"

void FFadeWidgetLatentAction::UpdateOperation(FLatentResponse& Response)
{
	if (!Widget.IsValid())
	{
		Response.DoneIf(true);
		return;
	}

	Elapsed += Response.ElapsedTime();
	const float Alpha = FMath::Clamp(Elapsed / Duration, 0.f, 1.f);

	const float NewOpacity = FMath::Lerp(StartOpacity, TargetOpacity, Alpha);
	Widget->SetRenderOpacity(FMath::Clamp(NewOpacity, 0.f, 1.f));

	const bool bFinished = (Alpha >= 1.f);
	Response.DoneIf(bFinished);

	if (bFinished)
	{
		Response.TriggerLink(LatentInfo.ExecutionFunction, LatentInfo.Linkage, LatentInfo.CallbackTarget);
	}
}
