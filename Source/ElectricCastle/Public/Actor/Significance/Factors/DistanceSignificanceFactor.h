// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "SignificanceFactor.h"
#include "DistanceSignificanceFactor.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UDistanceSignificanceFactor : public USignificanceFactor
{
	GENERATED_BODY()

public:
	virtual float CalculateSignificance(const AActor* ActorToEvaluate, const FTransform& ActorTransform, const FTransform& Viewpoint) const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Significance")
	float FullSignificanceRange = 500.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Significance")
	float InsignificantRange = 5000.f;
};
