// Copyright Alien Shores


#include "AI/Engagement/EngagementDirector.h"


// Sets default values
AEngagementDirector::AEngagementDirector()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void AEngagementDirector::RemoveParticipant_Implementation(AActor* Participant)
{
}

void AEngagementDirector::AddParticipant_Implementation(AActor* Participant)
{
}

void AEngagementDirector::RecalculateEngagement_Implementation()
{
}
