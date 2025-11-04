#pragma once

#include "CoreMinimal.h"
#include "SwitchDelegates.generated.h"

USTRUCT(BlueprintType)
struct FOnSwitchStatusChangedPayload
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> Switch;

	bool IsValid() const
	{
		return Switch != nullptr;
	}
};

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSwitchStatusChangedSignature, const FOnSwitchStatusChangedPayload&, Payload);
