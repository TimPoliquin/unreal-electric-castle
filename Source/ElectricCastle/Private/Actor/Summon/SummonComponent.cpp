// Copyright Alien Shores


#include "Actor/Summon/SummonComponent.h"


USummonComponent::USummonComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void USummonComponent::BeginPlay()
{
	Super::BeginPlay();
}


int32 USummonComponent::GetSummonCount() const
{
	return Actors.Num();
}

void USummonComponent::AddMinion(AActor* Minion)
{
	Track(Minion);
}
