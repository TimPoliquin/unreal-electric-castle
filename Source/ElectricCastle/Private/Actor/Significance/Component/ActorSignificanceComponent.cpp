// Copyright Alien Shores


#include "Actor/Significance/Component/ActorSignificanceComponent.h"

#include "Actor/Significance/WorldSignificanceSubsystem.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"


UActorSignificanceComponent::UActorSignificanceComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UActorSignificanceComponent::BeginPlay()
{
	Super::BeginPlay();
	if (UWorldSignificanceSubsystem* SignificanceSubsystem = UWorldSignificanceSubsystem::Get(GetOwner()))
	{
		if (!SignificanceTag.IsValid())
		{
			UE_LOG(LogElectricCastle, Error, TEXT("[%s:%s] No significance tag set for significant object"), *GetOwner()->GetName(), *GetName());
			return;
		}
		if (!IsValid(SignificanceCalculation))
		{
			UE_LOG(LogElectricCastle, Error, TEXT("[%s:%s] No significance calculation set for significant object"), *GetOwner()->GetName(), *GetName());
			return;
		}
		SignificanceSubsystem->RegisterObject(GetOwner(), SignificanceTag, SignificanceCalculation);
	}
}
