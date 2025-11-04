// Copyright Alien Shores


#include "Fishing/WaterRipple.h"


AWaterRipple::AWaterRipple()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWaterRipple::BeginPlay()
{
	Super::BeginPlay();
	if (bStartRippling)
	{
		PlayRipple(true);
	}
	else
	{
		GetRootComponent()->SetVisibility(false, true);
	}
}
