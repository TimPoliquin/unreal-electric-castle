#pragma once


#include "FormConfigTypes.generated.h"

UENUM(BlueprintType)
enum class EPlayerForm : uint8
{
	None = 0,
	Barbarian = 1,
	Egyptian = 2,
	Futureman = 3,
	Highlander = 4,
	Hippie = 5,
	Knight = 6,
	Native = 7,
	Roman = 8,
	Invalid_Max = 9
};
