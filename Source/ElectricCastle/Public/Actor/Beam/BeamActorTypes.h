#pragma once

#include "CoreMinimal.h"
#include "BeamActorTypes.generated.h"

UENUM(BlueprintType)
enum class EBeamCascadeType : uint8
{
	None,
	Linear
};

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FBeamCascadeLinearParams
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxDistance = 1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxCascades = 0;
};
