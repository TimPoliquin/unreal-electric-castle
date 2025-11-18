#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "PlayerFormDelegates.generated.h"

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FOnPlayerAvailableFormsChangedPayload
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> Player;
	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer AvailableForms;
};

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerAvailableFormsChangedSignature, const FOnPlayerAvailableFormsChangedPayload&, Payload);
