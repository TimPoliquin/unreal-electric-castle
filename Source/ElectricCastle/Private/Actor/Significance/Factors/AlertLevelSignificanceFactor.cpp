// Copyright Alien Shores


#include "Actor/Significance/Factors/AlertLevelSignificanceFactor.h"

#include "AI/Alert/AIAlertActor.h"
#include "AI/Alert/AIAlertComponent.h"

float UAlertLevelSignificanceFactor::CalculateSignificance(const AActor* ActorToEvaluate, const FTransform& ActorTransform, const FTransform& Viewpoint) const
{
	if (const UAIAlertComponent* AlertComponent = IAIAlertActor::GetAIAlertComponent(ActorToEvaluate))
	{
		if (AlertLevelSignificances.Contains(AlertComponent->GetCurrentAlertLevel()))
		{
			return AlertLevelSignificances[AlertComponent->GetCurrentAlertLevel()];
		}
	}
	return 0.f;
}
