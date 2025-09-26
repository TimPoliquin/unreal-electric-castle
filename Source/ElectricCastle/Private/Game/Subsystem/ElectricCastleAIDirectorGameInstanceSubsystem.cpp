// Copyright Alien Shores


#include "Game/Subsystem/ElectricCastleAIDirectorGameInstanceSubsystem.h"

#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Kismet/GameplayStatics.h"

UElectricCastleAIDirectorGameInstanceSubsystem* UElectricCastleAIDirectorGameInstanceSubsystem::Get(const UObject* WorldContextObject)
{
	if (!IsValid(WorldContextObject))
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Invalid world context object provided - cannot lookup game instance subsystem"), *FString("UElectricCastleAIDirectorGameInstanceSubsystem::Get"));
		return nullptr;
	}
	if (const UGameInstance* GameInstance = IsValid(WorldContextObject) ? UGameplayStatics::GetGameInstance(WorldContextObject) : nullptr)
	{
		return GameInstance->GetSubsystem<UElectricCastleAIDirectorGameInstanceSubsystem>();
	}
	UE_LOG(LogElectricCastle, Warning, TEXT("[%s] No subsystem found for context object: %s"), *FString("UElectricCastleAIDirectorGameInstanceSubsystem::Get"), *WorldContextObject->GetName());
	return nullptr;
}

TArray<AActor*> UElectricCastleAIDirectorGameInstanceSubsystem::GetActivePlayerActors()
{
	TArray<AActor*> Results;
	for (TWeakObjectPtr<AActor> Actor : ActivePlayerActors)
	{
		if (Actor.IsValid())
		{
			Results.Add(Actor.Get());
		}
	}
	return Results;
}
