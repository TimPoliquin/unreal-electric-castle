// Copyright Alien Shores


#include "Game/ElectricCastleGameInstance.h"

#include "Kismet/GameplayStatics.h"

UElectricCastleGameInstance* UElectricCastleGameInstance::Get(const UObject* WorldContextObject)
{
	if (IsValid(WorldContextObject))
	{
		return Cast<UElectricCastleGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	}
	return nullptr;
}
