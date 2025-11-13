#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AbilityChangeDelegates.generated.h"

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FOnAbilityChangedPayload
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> Owner = nullptr;
	UPROPERTY(BlueprintReadWrite)
	FGameplayTag AbilityTag = FGameplayTag::EmptyTag;
	UPROPERTY(BlueprintReadWrite)
	FGameplayTag InputTag = FGameplayTag::EmptyTag;
};

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnAbilityChangedSignature,
	const FOnAbilityChangedPayload&,
	Payload
);
