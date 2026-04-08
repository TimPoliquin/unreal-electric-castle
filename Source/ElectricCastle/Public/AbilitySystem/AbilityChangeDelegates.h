#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Data/AbilityInfo.h"
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
	UPROPERTY(BlueprintReadWrite)
	FElectricCastleAbilityInfo AbilityInfo;
};

struct FRemoveAbilityTracker
{
	FGameplayTag AbilityTag = FGameplayTag::EmptyTag;
	FGameplayTag InputTag = FGameplayTag::EmptyTag;
	FGameplayAbilitySpecHandle Handle = FGameplayAbilitySpecHandle();
};

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnAbilityChangedSignature,
	const FOnAbilityChangedPayload&,
	Payload
);
