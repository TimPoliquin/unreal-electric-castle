// Copyright Alien Shores

#include "Game/Subsystem/PlayerManager.h"

#include "Character/ElectricCastlePlayerCharacter.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Game/ElectricCastleGameInstance.h"

#include "Kismet/GameplayStatics.h"

#include "Player/ElectricCastlePlayerController.h"

UPlayerManager* UPlayerManager::Get(const UObject* WorldContextObject)
{
	if (const UElectricCastleGameInstance* GameInstance = UElectricCastleGameInstance::Get(WorldContextObject))
	{
		return GameInstance->GetSubsystem<UPlayerManager>();
	}
	return nullptr;
}

void UPlayerManager::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UPlayerManager::GetPlayerControllers(TArray<AElectricCastlePlayerController*>& Controllers) const
{
	for (const auto& [PlayerController, PlayerCharacter] : PlayerPairs)
	{
		if (PlayerController.IsValid())
		{
			Controllers.Add(PlayerController.Get());
		}
	}
}

void UPlayerManager::GetPlayerCharacters(TArray<AElectricCastlePlayerCharacter*>& Characters) const
{
	for (const auto& [PlayerController, PlayerCharacter] : PlayerPairs)
	{
		if (PlayerCharacter.IsValid())
		{
			Characters.Add(PlayerCharacter.Get());
		}
	}
}

bool UPlayerManager::IsPlayerCharacter(const AActor* Actor) const
{
	if (!IsValid(Actor) || !Actor->IsA(AElectricCastlePlayerCharacter::StaticClass()))
	{
		return false;
	}
	bool bIsPlayerCharacter = false;
	for (const auto& [PlayerController, PlayerCharacter] : PlayerPairs)
	{
		if (PlayerCharacter.IsValid() && Actor == PlayerController.Get())
		{
			bIsPlayerCharacter = true;
			break;
		}
	}
	return bIsPlayerCharacter;
}

bool UPlayerManager::IsPlayerController(const AActor* Actor) const
{
	if (!IsValid(Actor) || !Actor->IsA(AElectricCastlePlayerController::StaticClass()))
	{
		return false;
	}
	bool bIsPlayerController = false;
	for (const auto& [PlayerController, PlayerCharacter] : PlayerPairs)
	{
		if (PlayerController.IsValid() && Actor == PlayerController.Get())
		{
			bIsPlayerController = true;
			break;
		}
	}
	return bIsPlayerController;
}

AElectricCastlePlayerController* UPlayerManager::GetPlayerController(const AActor* Actor) const
{
	if (!IsValid(Actor))
	{
		return nullptr;
	}
	for (const auto& [PlayerController, PlayerCharacter] : PlayerPairs)
	{
		if (PlayerCharacter.IsValid() && PlayerCharacter.Get() == Actor)
		{
			return PlayerController.Get();
		}
	}
	return nullptr;
}

void UPlayerManager::RegisterPlayer(AElectricCastlePlayerController* PlayerController, AElectricCastlePlayerCharacter* PlayerCharacter)
{
	if (!IsValid(PlayerController))
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[%s][%s] Attempted to register an invalid PlayerController"), *GetNetModeName(GetWorld()->GetNetMode()), *GetName());
		return;
	}
	if (!IsValid(PlayerCharacter))
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[%s][%s] Attempted to register an invalid PlayerCharacter"), *GetNetModeName(GetWorld()->GetNetMode()), *GetName());
		return;
	}
	PlayerPairs.Add({PlayerController, PlayerCharacter});
	UE_LOG(LogElectricCastle, Log, TEXT("[%s][%s] Registered PlayerController: %s, PlayerCharacter: %s"), *GetNetModeName(GetWorld()->GetNetMode()), *GetName(), *PlayerController->GetName(),
	       *PlayerCharacter->GetName());
}

void UPlayerManager::UnregisterPlayer(AElectricCastlePlayerController* PlayerController)
{
	PlayerPairs.RemoveAll([PlayerController](const FPlayerPair& Pair) { return Pair.PlayerController == PlayerController; });
}

void UPlayerManager::UnregisterPlayer(AElectricCastlePlayerCharacter* PlayerCharacter)
{
	PlayerPairs.RemoveAll([PlayerCharacter](const FPlayerPair& Pair) { return Pair.PlayerCharacter == PlayerCharacter; });
}

void UPlayerManager::SetTimeDilation_Implementation(const float WorldMagnitude, const float PlayerMagnitude)
{
	UE_LOG(LogElectricCastle, Log, TEXT("[%s][%s] Setting time dilation to %f"), *GetNetModeName(GetWorld()->GetNetMode()), *GetName(), WorldMagnitude)
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), WorldMagnitude);
	for (const auto& [PlayerController, PlayerCharacter] : PlayerPairs)
	{
		PlayerCharacter->Multicast_SetTimeDilation(PlayerMagnitude);
	}
}

FString UPlayerManager::GetNetModeName(const ENetMode& NetMode)
{
	switch (NetMode)
	{
	case NM_Standalone: return TEXT("Standalone");
	case NM_Client: return TEXT("Client");
	case NM_ListenServer: return TEXT("ListenServer");
	case NM_DedicatedServer: return TEXT("DedicatedServer");
	default: return TEXT("Unknown");
	}
}
