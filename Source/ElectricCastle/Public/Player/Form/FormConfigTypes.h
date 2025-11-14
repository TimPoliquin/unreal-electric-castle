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
	Count = 9 UMETA(Hidden)
};

ENUM_RANGE_BY_FIRST_AND_LAST(EPlayerForm, EPlayerForm::Barbarian, EPlayerForm::Roman)
