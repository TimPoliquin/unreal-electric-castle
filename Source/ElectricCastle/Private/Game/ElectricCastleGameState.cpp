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

void AElectricCastleGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
	OnPlayerStateAdded.Broadcast(FGamePlayerStateAddedPayload(PlayerState, PlayerArray.IndexOfByKey(PlayerState)));
}

void AElectricCastleGameState::RemovePlayerState(APlayerState* PlayerState)
{
	const int32 PlayerIndex = PlayerArray.IndexOfByKey(PlayerState);
	Super::RemovePlayerState(PlayerState);
	OnPlayerStateRemoved.Broadcast(FGamePlayerStateRemovedPayload(PlayerState, PlayerIndex));
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
