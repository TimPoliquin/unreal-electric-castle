// Copyright Alien Shores


#include "Actor/Horde/HordeModeComponent.h"

#include "Actor/Horde/HordeModeConfig.h"
#include "Actor/Spawn/ElectricCastleSpawnBlueprintLibrary.h"
#include "Actor/Spawn/SpawnInterface.h"
#include "Actor/Spawn/SpawnEventPayloadDefinitions.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Utils/ArrayUtils.h"


// Sets default values for this component's properties
UHordeModeComponent::UHordeModeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UHordeModeComponent::HasNextRound() const
{
	return HordeModeConfig->HasConfigurationForRound(CurrentRound + 1);
}

TArray<FAuraSpawnParams> UHordeModeComponent::ConvertRewardsToSpawnParams(const FVector& Center, const float Radius, const TArray<FHordeRoundRewards>& Rewards)
{
	TArray<FAuraSpawnParams> SpawnParams;
	const int32 NumSpawnPoints = UHordeModeConfig::GetNumberOfRewardsToSpawn(Rewards);
	TArray<FTransform> SpawnTransforms = UElectricCastleSpawnBlueprintLibrary::GenerateSpawnLocations(Center, Radius, NumSpawnPoints);
	for (auto [RewardClass, Count] : Rewards)
	{
		SpawnParams.Add(FAuraSpawnParams(RewardClass, SpawnTransforms.Pop()));
	}
	return SpawnParams;
}

void UHordeModeComponent::PrepareNextRound()
{
	CurrentRound++;
}


void UHordeModeComponent::StartRound()
{
	if (!HordeModeConfig)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s.%s] No hoard mode configuration set!"), *GetOwner()->GetName(), *GetName());
		return;
	}
	if (!HordeModeConfig->HasConfigurationForRound(CurrentRound))
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s.%s] No hoard mode configuration for round %d"), *GetOwner()->GetName(), *GetName(), CurrentRound);
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s.%s] No hoard mode configuration for round %d"), *GetOwner()->GetName(), *GetName(), CurrentRound);
		return;
	}
	const FHordeRoundConfig& RoundConfig = HordeModeConfig->GetRoundConfig(CurrentRound);
	TArray<AActor*> AvailableSpawnPoints = SpawnPoints;
	UArrayUtils::ShuffleArray(AvailableSpawnPoints);
	for (int32 SpawnPointIdx = 0; SpawnPointIdx < RoundConfig.NumSpawnPoints; SpawnPointIdx++)
	{
		if (!AvailableSpawnPoints.IsValidIndex(SpawnPointIdx))
		{
			UE_LOG(LogElectricCastle, Warning,
			       TEXT("[%s.%s] Invalid spawn point index - not enough spawn points configured! Round: %d, NumSpawnPoints: %d"),
			       *GetOwner()->GetName(),
			       *GetName(),
			       CurrentRound,
			       SpawnPointIdx
			);
			break;
		}
		AActor* SpawnPointActor = AvailableSpawnPoints[SpawnPointIdx];
		if (ISpawnInterface* SpawnPointInterface = Cast<ISpawnInterface>(SpawnPointActor))
		{
			SpawnPointInterface->GetOnCountChangedDelegate().AddDynamic(this, &UHordeModeComponent::OnSpawnPointCountChanged);
			ActiveSpawnPoints.Add(SpawnPointActor);
			ISpawnInterface::Execute_BeginSpawning(SpawnPointActor, RoundConfig.NumEnemiesToSpawn, RoundConfig.SpawnDelay, RoundConfig.EnemyConfig);
		}
	}
}

void UHordeModeComponent::EndRound()
{
	ActiveSpawnPoints.Empty();
	const int32 CompletedRound = CurrentRound;
	TArray<FHordeRoundRewards> Rewards;
	if (HasNextRound())
	{
		PrepareNextRound();
		const int32 NextRound = CurrentRound;
		OnHordeModeRoundComplete.Broadcast(FOnHordeModeRoundCompletePayload(CompletedRound, NextRound, true, HordeModeConfig->GetRewards(CompletedRound)));
	}
	else
	{
		CurrentRound = 0;
		OnHordeModeRoundComplete.Broadcast(FOnHordeModeRoundCompletePayload(CompletedRound, -1, false, HordeModeConfig->GetRewards(CompletedRound)));
	}
}


void UHordeModeComponent::OnSpawnPointCountChanged(const FOnActorTrackerCountChangedPayload& Payload)
{
	if (Payload.IsEmpty() && Payload.Owner)
	{
		if (ISpawnInterface* SpawnInterface = Cast<ISpawnInterface>(Payload.Owner))
		{
			SpawnInterface->GetOnCountChangedDelegate().RemoveDynamic(this, &UHordeModeComponent::OnSpawnPointCountChanged);
		}
		ActiveSpawnPoints.RemoveAll([Payload](const AActor* SpawnPoint)
		{
			return SpawnPoint == Payload.Owner || !IsValid(SpawnPoint);
		});
		if (ActiveSpawnPoints.Num() <= 0)
		{
			EndRound();
		}
	}
}
