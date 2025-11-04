#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InventoryEvents.generated.h"

USTRUCT(BlueprintType)
struct FOnInventoryItemCountChangedPayload
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag ItemType = FGameplayTag::EmptyTag;
	UPROPERTY(BlueprintReadOnly)
	int32 OldValue = 0;
	UPROPERTY(BlueprintReadOnly)
	int32 NewValue = 0;

	bool IsValid() const
	{
		return ItemType.IsValid();
	}

	int32 GetDelta() const
	{
		return NewValue - OldValue;
	}

	bool IsItemUsedChange() const
	{
		return NewValue < OldValue;
	}

	bool IsItemAddedChange() const
	{
		return NewValue > OldValue;
	}

	bool IsNewItemAddedChange() const
	{
		return OldValue == 0 && NewValue > OldValue;
	}
};
