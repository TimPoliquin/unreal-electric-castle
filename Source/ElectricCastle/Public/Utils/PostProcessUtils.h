// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PostProcessUtils.generated.h"

class UMaterialInstanceDynamic;
class UMaterialInterface;
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
	UFUNCTION(BlueprintCallable, Category = "PostProcessUtils")
	static UMaterialInstanceDynamic* MakePostProcessMaterialDynamic(const UObject* WorldContextObject, UMaterialInterface* MaterialType);
};
