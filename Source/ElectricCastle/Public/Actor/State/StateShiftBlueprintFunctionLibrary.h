// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "StateShiftTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "StateShiftBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UStateShiftBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "StateShiftBlueprintFunctionLibrary", meta=( DefaultToSelf="Actor"))
	static void SetActorVisibility(AActor* Actor, const bool bEnableVisibility);
};
