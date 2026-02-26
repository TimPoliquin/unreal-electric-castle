#pragma once

#include "CoreMinimal.h"
#include "CinematicEvents.generated.h"

class UCinematicContextHandle;
class UCinematicContext;
class ULevelSequencePlayer;
class ULevelSequence;

UENUM(BlueprintType)
enum class ECinematicLifeCycle : uint8
{
	/** An invalid enum value marking the state has not been set **/
	None,
	/** The cinematic has begun **/
	Begin,
	/** The cinematic has ended **/
	End,
	/** An invalid enum value to mark the top bound **/
	Count
};

UENUM(BlueprintType)
enum class ECinematicPlayerRelocationMode : uint8
{
	None,
	Level,
	CinematicOverride,
	Count
};

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FCinematicLifeCycleEventPayload
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	ECinematicLifeCycle State = ECinematicLifeCycle::None;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UCinematicContextHandle> ContextHandle;
};

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCinematicLifeCycleEventSignature, const FCinematicLifeCycleEventPayload&, Payload);


USTRUCT(BlueprintType)
struct FCinematicContextEventPayload
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	ECinematicLifeCycle EventType = ECinematicLifeCycle::None;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UCinematicContext> CinematicContext;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCinematicContextEventSignature, const FCinematicContextEventPayload&, EventPayload);
