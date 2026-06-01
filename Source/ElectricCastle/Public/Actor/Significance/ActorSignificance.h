// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SignificanceTypes.h"
#include "UObject/Object.h"
#include "ActorSignificance.generated.h"

class USignificanceCalculation;
/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UActorSignificance : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TWeakObjectPtr<UObject> Actor;
	UPROPERTY()
	FGameplayTag SignificanceTag = FGameplayTag::EmptyTag;
	UPROPERTY()
	float Significance = 0.f;
	UPROPERTY()
	ESignificanceLevel SignificanceLevel = ESignificanceLevel::None;
};
