// Copyright Alien Shores


#include "Game/ElectricCastleGameModeBase.h"

#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Game/Subsystem/ElectricCastleLevelManager.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"


AActor* AElectricCastleGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	if (const APlayerController* PlayerController = Cast<APlayerController>(Player))
	{
		const UElectricCastleLevelManager* LevelSubsystem = UElectricCastleLevelManager::Get(PlayerController);
		const FName PlayerStartTag = LevelSubsystem
			                             ? LevelSubsystem->GetCurrentPlayerStartTag(PlayerController, true)
			                             : FName();
		UE_LOG(LogElectricCastle, Log, TEXT("[%s] Finding player start with tag: %s"), *GetName(), *PlayerStartTag.ToString());
		TArray<AActor*> Actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), Actors);
		if (Actors.Num() > 0)
		{
			AActor* SelectedActor = Actors[0];
			for (AActor* Actor : Actors)
			{
				if (APlayerStart* PlayerStart = Cast<APlayerStart>(Actor))
				{
					if (PlayerStart->PlayerStartTag == PlayerStartTag)
					{
						SelectedActor = PlayerStart;
						break;
					}
				}
			}
			return SelectedActor;
		}
	}
	return nullptr;
}

AElectricCastleGameModeBase* AElectricCastleGameModeBase::GetElectricCastleGameMode(const UObject* WorldContextObject)
{
	return Cast<AElectricCastleGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
}
