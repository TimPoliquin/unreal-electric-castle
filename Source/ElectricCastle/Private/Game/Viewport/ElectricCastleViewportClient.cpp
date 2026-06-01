// Copyright Alien Shores


#include "Game/Viewport/ElectricCastleViewportClient.h"

#include "Actor/Significance/WorldSignificanceSubsystem.h"

void UElectricCastleViewportClient::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (UWorldSignificanceSubsystem* WorldSignificanceSubsystem = UWorldSignificanceSubsystem::Get(GetWorld()))
	{
		WorldSignificanceSubsystem->ProcessSignificance(DeltaTime);
	}
}
