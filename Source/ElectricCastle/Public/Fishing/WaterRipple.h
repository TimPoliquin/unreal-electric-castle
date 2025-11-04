// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaterRipple.generated.h"

UCLASS(Blueprintable)
class ELECTRICCASTLE_API AWaterRipple : public AActor
{
	GENERATED_BODY()

public:
	AWaterRipple();
	UFUNCTION(BlueprintImplementableEvent)
	void PlayRipple(const bool bLoop);
	UFUNCTION(BlueprintImplementableEvent)
	void StopRipple();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bStartRippling = false;
};
