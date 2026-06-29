#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "UObject/ObjectPtr.h"
#include "AIPerceptionDataTypes.generated.h"

class AActor;

UENUM(BlueprintType)
enum class EAIPerceptionStimulusType : uint8
{
	None,
	Damage,
	Hearing,
	Sight,
	Touch,
	Unknown,
	Count UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FAIPerceivedActor
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<AActor> PerceivedActor;
	UPROPERTY(BlueprintReadOnly)
	FVector LastKnownLocation = FVector::ZeroVector;
	UPROPERTY(BlueprintReadOnly)
	TArray<EAIPerceptionStimulusType> ActiveStimuli;
	UPROPERTY(BlueprintReadOnly)
	bool bIsVisible = false;
	UPROPERTY(BlueprintReadOnly)
	bool bHasNotifiedOnPerceived = false;
	UPROPERTY(BlueprintReadOnly)
	bool bHasNotifiedOnUnperceived = false;

	FDelegateHandle VisibilityChangedDelegateHandle;

	bool IsValid() const;
	bool IsVisible() const;
	bool IsPerceived() const;
	bool HasAnyStimuli() const;
	void RemoveVisibilityChangedDelegate();
};

USTRUCT(BlueprintType)
struct FAIPerceptionAnyStartedEventPayload
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> PerceivedActor;
	UPROPERTY(BlueprintReadOnly)
	EAIPerceptionStimulusType StimulusType = EAIPerceptionStimulusType::None;
	UPROPERTY(BlueprintReadOnly)
	FVector StimulusLocation = FVector::ZeroVector;
};

USTRUCT(BlueprintType)
struct FAIPerceptionAnyEndedEventPayload
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> PerceivedActor;
};

USTRUCT(BlueprintType)
struct FAIPerceptionAllStartedEventPayload
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct FAIPerceptionAllEndedEventPayload
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	FVector LastKnownLocation = FVector::ZeroVector;
};


UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAIControllerPerceptionAnyStartedEventSignature, const FAIPerceptionAnyStartedEventPayload&, Payload);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAIControllerPerceptionAnyEndedEventSignature, const FAIPerceptionAnyEndedEventPayload&, Payload);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAIControllerPerceptionAllStartedEventSignature, const FAIPerceptionAllStartedEventPayload&, Payload);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAIControllerPerceptionAllEndedEventSignature, const FAIPerceptionAllEndedEventPayload&, Payload);
