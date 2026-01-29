#pragma once

#include "CoreMinimal.h"
#include "GameEvents.generated.h"

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FGamePlayerStateAddedPayload
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<APlayerState> PlayerState;
	UPROPERTY(BlueprintReadOnly)
	int32 PlayerIndex = 0;
};

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FGamePlayerStateRemovedPayload
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<APlayerState> PlayerState;
	UPROPERTY(BlueprintReadOnly)
	int32 PlayerIndex = 0;
};

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGamePlayerStateAddedSignature, const FGamePlayerStateAddedPayload&, Payload);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGamePlayerStateRemovedSignature, const FGamePlayerStateRemovedPayload&, Payload);
