#pragma once

#include "CoreMinimal.h"
#include "LockOnEvents.generated.h"

USTRUCT(BlueprintType)
struct FLockOnTargetPayload
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<AActor> TargetActor;
};

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLockOnTargetSignature, const FLockOnTargetPayload&, Payload);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLockOnReleaseSignature);
