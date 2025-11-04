#pragma once

#include "CoreMinimal.h"
#include "SpawnEventPayloadDefinitions.generated.h"

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FOnActorTrackerCountChangedPayload
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<AActor> Owner;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 PreviousCount = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 NewCount = 0;

	FOnActorTrackerCountChangedPayload()
	{
		Owner = nullptr;
		PreviousCount = 0;
		NewCount = 0;
	}

	FOnActorTrackerCountChangedPayload(AActor* InOwner, const int32 InPreviousCount, const int32 InNewCount)
	{
		Owner = InOwner;
		PreviousCount = InPreviousCount;
		NewCount = InNewCount;
	}

	bool IsEmpty() const
	{
		return NewCount == 0;
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAuraActorTrackerCountChangedDelegate, const FOnActorTrackerCountChangedPayload&, Payload);

UCLASS()
class ELECTRICCASTLE_API USpawnEventHelpers : public UObject
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool IsEmpty(const FOnActorTrackerCountChangedPayload& InPayload)
	{
		return InPayload.IsEmpty();
	}
};
