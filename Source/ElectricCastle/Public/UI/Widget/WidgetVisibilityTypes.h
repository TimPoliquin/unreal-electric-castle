#pragma once

#include "CoreMinimal.h"
#include "WidgetVisibilityTypes.generated.h"

UENUM(BlueprintType)
enum class EWidgetVisibilityState : uint8
{
	Hidden,
	Hiding,
	Showing,
	Shown
};
