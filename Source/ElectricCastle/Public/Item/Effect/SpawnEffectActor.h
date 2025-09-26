// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "SpawnEffectInterface.h"
#include "GameFramework/Actor.h"
#include "SpawnEffectActor.generated.h"

UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API ASpawnEffectActor : public AActor, public ISpawnEffectInterface
{
	GENERATED_BODY()

public:
	ASpawnEffectActor();

protected:
	virtual void BeginPlay() override;
	/** ISpawnEffectInterface Start **/
	virtual void PlaySpawnEffect_Implementation(const FAuraSpawnEffectParams& SpawnParams) override;
	/** ISpawnEffectInterface End **/

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FTransform CalculateTransform(const FAuraSpawnEffectParams& SpawnParams, const float Alpha, const float HeightAlpha, const float ScaleAlpha);

private:
	FTransform SpawnTransform;
};
