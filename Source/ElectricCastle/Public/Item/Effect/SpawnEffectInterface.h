// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SpawnEffectInterface.generated.h"

USTRUCT(BlueprintType)
struct FAuraSpawnEffectParams
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	FTransform SpawnTransform;
	UPROPERTY(BlueprintReadWrite)
	FTransform TargetTransform;
	UPROPERTY(BlueprintReadWrite)
	float HeightMultiplier = 1;
};

// This class does not need to be modified.
UINTERFACE()
class USpawnEffectInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API ISpawnEffectInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void PlaySpawnEffect(const FAuraSpawnEffectParams& SpawnParams);

	static void PlaySpawnEffect(AActor* Actor, const FTransform& SpawnTransform, const FTransform& TargetTransform, const float HeightMultiplier = 1.f);
	static AActor* CreateSpawnEffectActor(const UObject* WorldContextObject, TSubclassOf<AActor> EffectClass, AActor* Child, bool bStartHidden = false);
};
