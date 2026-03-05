// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FadeWidgetLibrary.generated.h"

class UWidget;
/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UFadeWidgetLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta=(Latent, LatentInfo="LatentInfo", WorldContext="WorldContextObject"))
	static void FadeWidget(
		UObject* WorldContextObject,
		UWidget* Widget,
		float TargetOpacity,
		float Duration,
		FLatentActionInfo LatentInfo);
};
