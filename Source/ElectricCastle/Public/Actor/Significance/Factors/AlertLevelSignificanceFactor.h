// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "SignificanceFactor.h"
#include "AI/Alert/AlertTypes.h"
#include "AlertLevelSignificanceFactor.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UAlertLevelSignificanceFactor : public USignificanceFactor
{
	GENERATED_BODY()

public:
	virtual float CalculateSignificance(const AActor* ActorToEvaluate, const FTransform& ActorTransform, const FTransform& Viewpoint) const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Significance")
	TMap<EAlertLevel, float> AlertLevelSignificances;
};
