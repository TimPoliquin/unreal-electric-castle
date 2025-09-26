#pragma once
#include "EquipmentTypes.h"
#include "GameplayTagContainer.h"

#include "EquipmentEvents.generated.h"

USTRUCT(BlueprintType)
struct FEquipmentDelegatePayload
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	EEquipmentSlot EquipmentSlot = EEquipmentSlot::None;
	UPROPERTY(BlueprintReadWrite)
	FGameplayTag EquipmentTag;
};

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEquipmentDelegateSignature, const FEquipmentDelegatePayload&, Payload);
