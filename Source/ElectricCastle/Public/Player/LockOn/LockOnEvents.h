#pragma once

#include "CoreMinimal.h"
#include "LockOnEvents.generated.h"

UENUM(BlueprintType)
enum class ELockOnLevel : uint8
{
	None,
	Soft,
	Hard,
	Count UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FLockOnTargetPayload
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<AActor> TargetActor;
	UPROPERTY(BlueprintReadWrite)
	ELockOnLevel LockOnLevel = ELockOnLevel::None;
};

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLockOnTargetSignature, const FLockOnTargetPayload&, Payload);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLockOnReleaseSignature);
