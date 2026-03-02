#pragma once

#include "CoreMinimal.h"
#include "PuzzleManagerTypes.generated.h"

UENUM(BlueprintType)
enum class EPuzzleManagerState : uint8
{
	Inactive,
	Active,
	Solved
};

UENUM(BlueprintType)
enum class EPuzzleActionTriggerMode : uint8
{
	Activate,
	Deactivate
};
