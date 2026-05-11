// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AlertTypes.generated.h"

UENUM(BlueprintType)
enum class EAlertLevel : uint8
{
	Idle,
	Suspicious,
	Alerted,
	Count
};

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FAlertLevelChangePayload
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> Owner;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> AlertedBy;
	UPROPERTY(BlueprintReadWrite)
	FVector LastKnownLocation = FVector::ZeroVector;
	UPROPERTY(BlueprintReadWrite)
	EAlertLevel PreviousAlertLevel = EAlertLevel::Idle;
	UPROPERTY(BlueprintReadWrite)
	EAlertLevel NewAlertLevel = EAlertLevel::Idle;
	UPROPERTY(BlueprintReadWrite)
	float RawAlertLevel = 0.f;
};

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FAlertTargetPerceivedChangePayload
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> Owner;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> AlertedBy;
	UPROPERTY(BlueprintReadWrite)
	FVector LastKnownLocation = FVector::ZeroVector;
	UPROPERTY(BlueprintReadWrite)
	bool bCanPerceive = false;
};

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAlertLevelChanged, const FAlertLevelChangePayload&, Payload);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAlertTargetPerceiveChanged, const FAlertTargetPerceivedChangePayload&, Payload);
