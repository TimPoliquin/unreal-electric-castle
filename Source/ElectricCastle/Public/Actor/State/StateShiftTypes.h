#pragma once

#include "CoreMinimal.h"
#include "StateShiftTypes.generated.h"

UENUM(BlueprintType)
enum class EStateShiftState : uint8
{
	Normal,
	Psychedelic
};

UENUM(BlueprintType)
enum class EStateShiftReactionType : uint8
{
	None,
	Custom,
	Visibility_Show,
	Visibility_Hide,
	Visibility_Mask,
	Visibility_FadeIn,
	Visibility_FadeOut,
	Collision_Enable,
	Collision_Disable,
};


USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FStateShiftRequest
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<AActor> Instigator = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Radius = 0.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EStateShiftState NewState = EStateShiftState::Normal;
};

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FStateShiftStateChangedPayload
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<AActor> Instigator = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Radius = 0.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EStateShiftState OldState = EStateShiftState::Normal;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EStateShiftState NewState = EStateShiftState::Normal;

	static FStateShiftStateChangedPayload FromRequest(
		const FStateShiftRequest& Request,
		const EStateShiftState OldState
	)
	{
		return FStateShiftStateChangedPayload(Request.Instigator, Request.Radius, OldState, Request.NewState);
	}
};

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnStateShiftStateChangedSignature,
	const FStateShiftStateChangedPayload&,
	Payload
);
