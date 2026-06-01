#pragma once

#include "SignificanceTypes.generated.h"

UENUM(BlueprintType)
enum class ESignificanceLevel : uint8
{
	None UMETA(Hidden),
	Insignificant,
	PartiallySignificant,
	FullySignificant
};
