// Copyright Alien Shores


#include "Game/ElectricCastleGameState.h"

#include "Kismet/GameplayStatics.h"
#include "Player/InventoryComponent.h"
#include "Player/Progression/ProgressionComponent.h"

AElectricCastleGameState::AElectricCastleGameState()
{
	SetNetUpdateFrequency(100.f);
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("PlayerInventoryComponent"));
	ProgressionComponent = CreateDefaultSubobject<UProgressionComponent>(TEXT("ProgressionComponent"));
}

void AElectricCastleGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

AElectricCastleGameState* AElectricCastleGameState::Get(const UObject* WorldContextObject)
{
	if (IsValid(WorldContextObject))
	{
		return Cast<AElectricCastleGameState>(UGameplayStatics::GetGameState(WorldContextObject));
	}
	return nullptr;
}

UInventoryComponent* AElectricCastleGameState::GetInventoryComponent() const
{
	return InventoryComponent;
}

UProgressionComponent* AElectricCastleGameState::GetProgressionComponent() const
{
	return ProgressionComponent;
}
