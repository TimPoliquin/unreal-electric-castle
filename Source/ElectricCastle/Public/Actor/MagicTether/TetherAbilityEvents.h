#pragma once

#include "CoreMinimal.h"
#include "TetherAbilityEvents.generated.h"

struct FTetherLimitExceededPayload;
UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTetherLimitExceededSignature, const FTetherLimitExceededPayload&, Payload);
