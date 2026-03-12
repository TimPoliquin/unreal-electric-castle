// Copyright Alien Shores


#include "Player/Utils/PlayerFunctionLibrary.h"

#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Kismet/GameplayStatics.h"
#include "Player/ElectricCastlePlayerController.h"
#include "Player/PlayerEquipmentComponent.h"
#include "Player/Aim/AimController.h"
#include "Player/Equipment/EquipmentManagerInterface.h"

AElectricCastlePlayerController* UPlayerFunctionLibrary::GetPlayerController(UObject* WorldContextObject)
{
	if (!IsValid(WorldContextObject))
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[UPlayerFunctionLibrary::GetPlayerController] Invalid world context object"));
		return nullptr;
	}
	if (WorldContextObject->IsA<AElectricCastlePlayerController>())
	{
		return Cast<AElectricCastlePlayerController>(WorldContextObject);
	}
	if (const APawn* Pawn = Cast<APawn>(WorldContextObject))
	{
		if (Pawn->GetController() && Pawn->GetController()->IsA<AElectricCastlePlayerController>())
		{
			return Cast<AElectricCastlePlayerController>(Pawn->GetController());
		}
	}
	if (const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject))
	{
		return Cast<AElectricCastlePlayerController>(GameInstance->GetFirstLocalPlayerController());
	}
	return nullptr;
}

ULocalPlayer* UPlayerFunctionLibrary::GetLocalPlayer(UObject* WorldContextObject)
{
	if (!IsValid(WorldContextObject))
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[UPlayerFunctionLibrary::GetLocalPlayer] Invalid world context object"));
		return nullptr;
	}
	if (const AElectricCastlePlayerController* PlayerController = GetPlayerController(WorldContextObject))
	{
		return PlayerController->GetLocalPlayer();
	}
	UE_LOG(LogElectricCastle, Error, TEXT("[UPlayerFunctionLibrary::GetLocalPlayer] Unable to find local player from world context object"));
	return nullptr;
}

bool UPlayerFunctionLibrary::IsPlayerAiming(const AActor* Actor)
{
	if (const UAimController* AimController = IAimActorInterface::GetAimController(Actor))
	{
		return AimController->IsAiming();
	}
	return false;
}

bool UPlayerFunctionLibrary::HasWeaponEquipped(const AActor* Actor)
{
	if (const UPlayerEquipmentComponent* EquipmentComponent = IEquipmentManagerInterface::GetEquipmentComponent(Actor))
	{
		return EquipmentComponent->HasAnyWeaponEquipped();
	}
	return false;
}
