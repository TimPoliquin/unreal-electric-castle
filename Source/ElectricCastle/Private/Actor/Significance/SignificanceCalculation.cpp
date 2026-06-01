// Copyright Alien Shores


#include "Actor/Significance/SignificanceCalculation.h"

#include "Actor/Significance/Factors/SignificanceFactor.h"
#include "Interaction/CombatInterface.h"


float USignificanceCalculation::CalculateSignificance(const USignificanceManager::FManagedObjectInfo* ManagedObjectInfo, const FTransform& Viewpoint) const
{
	if (!IsValid(ManagedObjectInfo->GetObject()) || ICombatInterface::IsDead(ManagedObjectInfo->GetObject()))
	{
		return -1.f;
	}
	const AActor* ActorToEvaluate = Cast<AActor>(ManagedObjectInfo->GetObject());
	if (!IsValid(ActorToEvaluate))
	{
		return -1.f;
	}
	const FTransform& ObjectTransform = ActorToEvaluate->GetActorTransform();
	float TotalSignificance = 0.f;
	float TotalWeight = 0.f;
	float TotalPenalty = 1.f;
	for (const FWeightedSignificanceFactor& SignificanceFactor : SignificanceFactors)
	{
		const float FactorSignificance = SignificanceFactor.SignificanceFactor->CalculateSignificance(ActorToEvaluate, ObjectTransform, Viewpoint);
		TotalSignificance += FMath::Square(FactorSignificance) * SignificanceFactor.Weight;
		TotalWeight += SignificanceFactor.Weight;
	}
	for (const FWeightedSignificanceFactor& PenaltyFactor : PenaltyFactors)
	{
		const float Penalty = PenaltyFactor.SignificanceFactor->CalculateSignificance(ActorToEvaluate, ObjectTransform, Viewpoint);
		if (Penalty > 0.f)
		{
			TotalPenalty *= FMath::Lerp(1.f - PenaltyFactor.Weight, 1.f, Penalty);
		}
	}
	const float BaseSignificance = TotalWeight > 0.f ? FMath::Sqrt(TotalSignificance / TotalWeight) : 0.f;
	return BaseSignificance * TotalPenalty;
}
