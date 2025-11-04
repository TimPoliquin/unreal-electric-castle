#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/ElectricCastleAbilitySystemTypes.h"
#include "AttributeChangeDelegates.generated.h"

USTRUCT(BlueprintType)
struct FAuraFloatAttributeChangedPayload
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	FGameplayTag AttributeTag = FGameplayTag::EmptyTag;
	UPROPERTY(BlueprintReadWrite)
	float OldValue = 0.f;
	UPROPERTY(BlueprintReadWrite)
	float NewValue = 0.f;

	bool IsChanged() const
	{
		return OldValue != NewValue;
	}

	static FAuraFloatAttributeChangedPayload CreateBroadcastPayload(const FGameplayTag& InAttributeTag, const float Value)
	{
		FAuraFloatAttributeChangedPayload Result;
		Result.AttributeTag = InAttributeTag;
		Result.OldValue = Value;
		Result.NewValue = Value;
		return Result;
	}
};

USTRUCT(BlueprintType)
struct FAuraIntAttributeChangedPayload
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	FGameplayTag AttributeTag = FGameplayTag::EmptyTag;
	UPROPERTY(BlueprintReadWrite)
	int32 OldValue = 0;
	UPROPERTY(BlueprintReadWrite)
	int32 NewValue = 0;

	bool IsChanged() const
	{
		return OldValue != NewValue;
	}

	static FAuraIntAttributeChangedPayload CreateBroadcastPayload(const FGameplayTag& InAttributeTag, const float Value)
	{
		FAuraIntAttributeChangedPayload Result;
		Result.AttributeTag = InAttributeTag;
		Result.OldValue = Value;
		Result.NewValue = Value;
		return Result;
	}
};


UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature, const FAuraFloatAttributeChangedPayload&, Payload);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChangedSignature, const FAuraIntAttributeChangedPayload&, Payload);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnPlayerAbilityStatusChangedSignature,
	const int32,
	PlayerLevel,
	const TArray<FAbilityTagStatus>&,
	AbilityStatuses
);
