// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SignificanceManager.h"
#include "SignificanceCalculation.generated.h"

class USignificanceFactor;

USTRUCT(BlueprintType)
struct FWeightedSignificanceFactor
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin=0, ClampMax=1, UIMin=0, UIMax=1))
	float Weight = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Export)
	TObjectPtr<USignificanceFactor> SignificanceFactor;
};

/**
 * 
 */
UCLASS(BlueprintType, DefaultToInstanced, EditInlineNew)
class ELECTRICCASTLE_API USignificanceCalculation : public UObject
{
	GENERATED_BODY()

public:
	virtual float CalculateSignificance(const USignificanceManager::FManagedObjectInfo* ManagedObjectInfo, const FTransform& Viewpoint) const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Significance")
	TArray<FWeightedSignificanceFactor> SignificanceFactors;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Significance")
	TArray<FWeightedSignificanceFactor> PenaltyFactors;
};
