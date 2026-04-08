// Copyright Alien Shores


#include "Input/Utils/PlayerInputFunctionLibrary.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Input/ElectricCastleInputComponent.h"
#include "Player/Utils/PlayerFunctionLibrary.h"

UEnhancedInputLocalPlayerSubsystem* UPlayerInputFunctionLibrary::GetEnhancedInputSubsystem(UObject* WorldContextObject)
{
	return UPlayerFunctionLibrary::GetPlayerSubsystem<UEnhancedInputLocalPlayerSubsystem>(WorldContextObject);
}

void UPlayerInputFunctionLibrary::AddInputMappingContext(UObject* WorldContextObject, const UInputMappingContext* InputMappingContext, const int32 Priority)
{
	if (!IsValid(InputMappingContext))
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[UPlayerInputFunctionLibrary::AddInputMappingContext] Invalid input mapping context"));
		return;
	}
	if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = GetEnhancedInputSubsystem(WorldContextObject))
	{
		InputSubsystem->AddMappingContext(InputMappingContext, Priority);
	}
	else
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[UPlayerInputFunctionLibrary::AddInputMappingContext] Unable to find input subsystem"));
	}
}

void UPlayerInputFunctionLibrary::RemoveInputMappingContext(UObject* WorldContextObject, const UInputMappingContext* InputMappingContext)
{
	if (!IsValid(InputMappingContext))
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[UPlayerInputFunctionLibrary::RemoveInputMappingContext] Invalid input mapping context"));
		return;
	}
	if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = GetEnhancedInputSubsystem(WorldContextObject))
	{
		InputSubsystem->RemoveMappingContext(InputMappingContext);
	}
	else
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[UPlayerInputFunctionLibrary::RemoveInputMappingContext] Unable to find input subsystem"));
	}
}

UElectricCastleInputComponent* UPlayerInputFunctionLibrary::GetPlayerInputComponent(AActor* PlayerActor)
{
	if (!IsValid(PlayerActor))
	{
		return nullptr;
	}
	if (UElectricCastleInputComponent* InputComponent = Cast<UElectricCastleInputComponent>(PlayerActor->InputComponent))
	{
		return InputComponent;
	}
	return nullptr;
}
