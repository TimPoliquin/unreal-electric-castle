#pragma once

#include "CoreMinimal.h"
#include "BeamActorTypes.generated.h"

UENUM(BlueprintType)
enum class EBeamCascadeType : uint8
{
	None,
	Linear,
	Web
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

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FBeamCascadeWebParams
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxDistance = 1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxCascades = 0;
};
