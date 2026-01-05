// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PostProcessUtils.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UPostProcessUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "PostProcessUtils")
	static void SetMotionBlurAmount(const UObject* WorldContextObject, const float Amount = 0.5f);
};
