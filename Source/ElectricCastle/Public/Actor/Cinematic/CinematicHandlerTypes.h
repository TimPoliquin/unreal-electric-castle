#pragma once

#include "CinematicHandlerTypes.generated.h"

UENUM(BlueprintType)
enum class ECinematicFilter : uint8
{
	None UMETA(DisplayName = "React to no cinematics"),
	All UMETA(DisplayName = "React to all cinematics"),
	WithTags UMETA(DisplayName = "React to cinematics with tags"),
	IgnoreTags UMETA(DisplayName = "Ignore cinematics with tags")
};

UENUM(BlueprintType)
enum class ECinematicVisibilityChange : uint8
{
	DoNothing,
	Hide,
	Show,
	Count
};

UENUM(BlueprintType)
enum class ECinematicTickChange : uint8
{
	DoNothing,
	StopTick,
	StartTick,
	Count
};

UENUM(BlueprintType)
enum class ECinematicTransformChange : uint8
{
	DoNothing,
	Move,
	Count
};

UENUM(BlueprintType)
enum class ECinematicStateChange :uint8
{
	DoNothing,
	Activate,
	Deactivate,
	Count
};
