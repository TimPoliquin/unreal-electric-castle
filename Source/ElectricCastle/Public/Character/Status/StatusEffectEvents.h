#pragma once

#include "CoreMinimal.h"
#include "StatusEffectTypes.h"
#include "StatusEffectEvents.generated.h"

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatusEffectAddedSignature, const FOnStatusEffectAddedPayload&, Payload);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatusEffectRemovedSignature, const FOnStatusEffectRemovedPayload&, Payload);
