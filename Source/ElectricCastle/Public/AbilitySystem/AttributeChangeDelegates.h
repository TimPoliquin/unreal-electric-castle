#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/ElectricCastleAbilitySystemTypes.h"
#include "AttributeChangeDelegates.generated.h"

USTRUCT(BlueprintType)
struct FFloatAttributeChangedPayload
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

	static FFloatAttributeChangedPayload CreateBroadcastPayload(const FGameplayTag& InAttributeTag, const float Value)
	{
		FFloatAttributeChangedPayload Result;
		Result.AttributeTag = InAttributeTag;
		Result.OldValue = Value;
		Result.NewValue = Value;
		return Result;
	}
};

USTRUCT(BlueprintType)
struct FIntAttributeChangedPayload
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

	static FIntAttributeChangedPayload CreateBroadcastPayload(const FGameplayTag& InAttributeTag, const float Value)
	{
		FIntAttributeChangedPayload Result;
		Result.AttributeTag = InAttributeTag;
		Result.OldValue = Value;
		Result.NewValue = Value;
		return Result;
	}
};


UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnAttributeChangedSignature,
	const FFloatAttributeChangedPayload&,
	Payload
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChangedSignature, const FIntAttributeChangedPayload&, Payload);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnPlayerAbilityStatusChangedSignature,
	const int32,
	PlayerLevel,
	const TArray<FAbilityTagStatus>&,
	AbilityStatuses
);
