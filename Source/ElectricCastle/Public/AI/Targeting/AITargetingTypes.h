#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UObject/ObjectPtr.h"
#include "AITargetingTypes.generated.h"

class UAITargetingComponent;

USTRUCT(BlueprintType)
struct FAITargetChangedPayload
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> Owner;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UAITargetingComponent> AITargetingComponent;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> OldTarget;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> NewTarget;
};
