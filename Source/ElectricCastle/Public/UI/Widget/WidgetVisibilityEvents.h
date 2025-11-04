#pragma once
#include "WidgetVisibilityEvents.generated.h"

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWidgetEventSignature, AActor*, Player);
