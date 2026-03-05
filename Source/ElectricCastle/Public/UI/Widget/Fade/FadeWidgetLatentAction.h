// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "LatentActions.h"

class UWidget;

class FFadeWidgetLatentAction : public FPendingLatentAction
{
public:
	FFadeWidgetLatentAction(
		UWidget* InWidget,
		float InStartOpacity,
		float InTargetOpacity,
		float InDuration,
		const FLatentActionInfo& InLatentInfo)
		: Widget(InWidget)
		  , StartOpacity(InStartOpacity)
		  , TargetOpacity(InTargetOpacity)
		  , Duration(FMath::Max(InDuration, KINDA_SMALL_NUMBER))
		  , Elapsed(0.f)
		  , LatentInfo(InLatentInfo)
	{
	}

	virtual void UpdateOperation(FLatentResponse& Response) override;

private:
	TWeakObjectPtr<UWidget> Widget;
	float StartOpacity;
	float TargetOpacity;
	float Duration;
	float Elapsed;
	FLatentActionInfo LatentInfo;
};
