// Copyright Alien Shores


#include "Actor/Track/TrackActor.h"

#include "Actor/Track/TrackFollowingActor.h"
#include "Actor/Track/TrackFollowingComponent.h"
#include "Components/SplineComponent.h"


ATrackActor::ATrackActor()
{
	PrimaryActorTick.bCanEverTick = false;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
	Track = CreateDefaultSubobject<USplineComponent>(TEXT("Track"));
	Track->SetupAttachment(GetRootComponent());
}

void ATrackActor::BeginPlay()
{
	Super::BeginPlay();
	SetTrackOnTrackFollowingActor();
}


void ATrackActor::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ATrackActor, TrackFollowingActor))
	{
		if (UTrackFollowingComponent* TrackFollowingComponent = TrackFollowingActor ? TrackFollowingActor->GetComponentByClass<UTrackFollowingComponent>() : nullptr)
		{
			TrackFollowingComponent->SetTrack(Track);
		}
	}
}

void ATrackActor::SetTrackOnTrackFollowingActor() const
{
	if (UTrackFollowingComponent* TrackFollowingComponent = ITrackFollowingActor::GetTrackFollowingComponent(TrackFollowingActor))
	{
		TrackFollowingComponent->SetTrack(Track);
	}
}
