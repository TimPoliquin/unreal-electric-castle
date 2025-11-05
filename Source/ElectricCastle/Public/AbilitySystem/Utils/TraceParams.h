// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "TraceParams.generated.h"

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FLineTraceParams
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TraceDistance = 5000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Pawn;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDebug = false;
};

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FSphereTraceParams
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TraceDistance = 5000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TraceRadius = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Pawn;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDebug = false;
};
