// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ElectricCastleAIDirectorGameInstanceSubsystem.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API UElectricCastleAIDirectorGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static UElectricCastleAIDirectorGameInstanceSubsystem* Get(const UObject* WorldContextObject);

	void RegisterActivePlayer(AActor* Actor)
	{
		ActivePlayerActors.Add(Actor);
	}

	void UnregisterActivePlayer(AActor* Actor)
	{
		ActivePlayerActors.Remove(Actor);
	}

	TArray<AActor*> GetActivePlayerActors();

private:
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> ActivePlayerActors;
};
