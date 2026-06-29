// Copyright Alien Shores


#include "Actor/Encounter/EncounterBlueprintFunctionLibrary.h"

#include "AI/Perception/AIPerceptionManager.h"
#include "AI/Perception/AIPerceptionManagerActor.h"

TArray<AActor*> UEncounterBlueprintFunctionLibrary::GetAllActorsPerceivedByActors(const TArray<AActor*>& PerceivingActors)
{
	TArray<AActor*> PerceivedActors;
	for (const AActor* Actor : PerceivingActors)
	{
		if (const UAIPerceptionManager* PerceptionManager = IAIPerceptionManagerActor::GetAIPerceptionManager(Actor))
		{
			for (AActor* PerceivedActor : PerceptionManager->GetCurrentPerceivedActors())
			{
				PerceivedActors.AddUnique(PerceivedActor);
			}
		}
	}
	return PerceivedActors;
}

TArray<AActor*> UEncounterBlueprintFunctionLibrary::GetAllActorsPerceivedByWeakActors(const TArray<TWeakObjectPtr<AActor>>& PerceivingActors)
{
	TArray<AActor*> PerceivedActors;
	for (const TWeakObjectPtr WeakActor : PerceivingActors)
	{
		if (!WeakActor.IsValid())
		{
			continue;
		}
		if (const UAIPerceptionManager* PerceptionManager = IAIPerceptionManagerActor::GetAIPerceptionManager(WeakActor.Get()))
		{
			for (AActor* PerceivedActor : PerceptionManager->GetCurrentPerceivedActors())
			{
				PerceivedActors.AddUnique(PerceivedActor);
			}
		}
	}
	return PerceivedActors;
}

AActor* UEncounterBlueprintFunctionLibrary::GetClosestActor(const AActor* SourceActor, const TArray<AActor*>& Actors, FVector& OutLocation)
{
	if (!IsValid(SourceActor) || Actors.IsEmpty())
	{
		return nullptr;
	}
	const FVector& SourceLocation = SourceActor->GetActorLocation();
	AActor* ClosestActor = nullptr;
	float ClosestDistance = FLT_MAX;
	for (AActor* Actor : Actors)
	{
		FVector ActorLocation = Actor->GetActorLocation();
		if (const float Distance = FVector::DistSquared(SourceLocation, ActorLocation); Distance < ClosestDistance)
		{
			ClosestDistance = Distance;
			OutLocation = ActorLocation;
			ClosestActor = Actor;
		}
	}
	return ClosestActor;
}
