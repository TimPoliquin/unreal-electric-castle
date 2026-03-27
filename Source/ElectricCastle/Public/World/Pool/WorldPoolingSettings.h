// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "WorldPoolingSettings.generated.h"

USTRUCT(BlueprintType)
struct FPoolWarmUpConfig
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AActor> ActorClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 PreloadSize = 20;
};

UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="World Pooling"))
class ELECTRICCASTLE_API UWorldPoolingSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Config, Category="Pooling")
	TArray<FPoolWarmUpConfig> WarmUpConfigs;
};
