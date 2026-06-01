// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SignificanceFactor.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, DefaultToInstanced, EditInlineNew)
class ELECTRICCASTLE_API USignificanceFactor : public UObject
{
	GENERATED_BODY()

public:
	virtual float CalculateSignificance(const AActor* ActorToEvaluate, const FTransform& ActorTransform, const FTransform& Viewpoint) const;
};
