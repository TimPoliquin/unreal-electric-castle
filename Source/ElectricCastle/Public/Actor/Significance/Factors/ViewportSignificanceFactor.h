// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "SignificanceFactor.h"
#include "ViewportSignificanceFactor.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UViewportSignificanceFactor : public USignificanceFactor
{
	GENERATED_BODY()

public:
	virtual float CalculateSignificance(const AActor* ActorToEvaluate, const FTransform& ActorTransform, const FTransform& Viewpoint) const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Significance")
	float CosHalfFOVThreshold = 0.55f;
};
