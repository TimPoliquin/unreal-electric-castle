#pragma once

#include "CoreMinimal.h"
#include "TetherAbilityTypes.generated.h"

class UTetherAbilityComponent;

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FTetherLimitExceededPayload
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> TetherOwner = nullptr;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> TetherTarget = nullptr;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UTetherAbilityComponent> TetherAbilityComponent = nullptr;
	UPROPERTY(BlueprintReadWrite)
	float Distance = 0.f;
	UPROPERTY(BlueprintReadWrite)
	float TetherLength = 0.f;
};
