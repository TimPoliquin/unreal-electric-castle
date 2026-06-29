// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UObject/ObjectPtr.h"
#include "AlertTypes.generated.h"

UENUM(BlueprintType)
enum class EAlertLevel : uint8
{
	Idle,
	Suspicious,
	Alerted,
	Count UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FAlertLevelChangePayload
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> Owner;
	UPROPERTY(BlueprintReadWrite)
	EAlertLevel PreviousAlertLevel = EAlertLevel::Idle;
	UPROPERTY(BlueprintReadWrite)
	EAlertLevel NewAlertLevel = EAlertLevel::Idle;
	UPROPERTY(BlueprintReadWrite)
	float RawAlertLevel = 0.f;
};


UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAlertLevelChanged, const FAlertLevelChangePayload&, Payload);
