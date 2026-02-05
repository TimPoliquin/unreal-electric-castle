// Copyright Alien Shores


#include "Actor/Spawn/SpawnManager.h"

#include "Actor/Spawn/ActorTrackerComponent.h"
#include "Actor/Spawn/EnemySpawnConfig.h"
#include "Actor/Summon/SummonComponent.h"
#include "Actor/Summon/SummoningActor.h"
#include "Character/ElectricCastleEnemyCharacter.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Engine/TargetPoint.h"
#include "Utils/ArrayUtils.h"
#include "Utils/RandUtils.h"


ASpawnManager::ASpawnManager()
{
	PrimaryActorTick.bCanEverTick = false;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root Component")));
	SpawnPointComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("SpawnPoint Component"));
	SpawnPointComponent->SetChildActorClass(ATargetPoint::StaticClass());
	SpawnPointComponent->SetupAttachment(GetRootComponent());
	EnemyTrackerComponent = CreateDefaultSubobject<UActorTrackerComponent>(TEXT("Enemy Tracker Component"));
}

FActorTrackerCountChangedDelegate& ASpawnManager::GetOnCountChangedDelegate()
{
	return EnemyTrackerComponent->OnCountChanged;
}

void ASpawnManager::BeginSpawning_Implementation(const int32 NumEnemies, const FRandRange& SpawnDelay, const TArray<FEnemySpawnConfig>& SpawnClasses)
{
	InitializeEnemies(NumEnemies, SpawnClasses, Enemies);
	SpawnEnemiesWithDelay(SpawnDelay);
}

int32 ASpawnManager::GetSpawnCount_Implementation() const
{
	return EnemyTrackerComponent->GetActorCount();
}

void ASpawnManager::BeginPlay()
{
	Super::BeginPlay();
	EnemyTrackerComponent->OnCountChanged.AddDynamic(this, &ASpawnManager::OnEnemyCountChanged);
}

void ASpawnManager::InitializeEnemies(const int32 NumToSpawn, const TArray<FEnemySpawnConfig>& SpawnClasses, TArray<AElectricCastleEnemyCharacter*>& OutEnemies)
{
	for (int32 Idx = 0; Idx < NumToSpawn; Idx++)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		const FEnemySpawnConfig& SpawnConfig = UArrayUtils::GetRandomElement(SpawnClasses);
		if (AElectricCastleEnemyCharacter* Enemy = SpawnPointComponent->GetWorld()->SpawnActorDeferred<AElectricCastleEnemyCharacter>(
			SpawnConfig.EnemyClass,
			SpawnPointComponent->GetChildActor()->GetActorTransform(),
			this,
			nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		))
		{
			Enemy->SetLevel(SpawnConfig.EnemyLevel);
			Enemy->SetVisible(false);
			OutEnemies.Add(Enemy);
		}
		else
		{
			UE_LOG(LogElectricCastle, Error, TEXT("[%s] Failed to spawn enemy"), *GetName());
		}
	}
}

void ASpawnManager::FinishSpawningEnemy(AElectricCastleEnemyCharacter* Enemy)
{
	EnemyTrackerComponent->Track(Enemy);
	Enemy->FinishSpawning(SpawnPointComponent->GetChildActor()->GetActorTransform());
	Enemy->SpawnDefaultController();
	Enemy->SetVisible(true);
	if (USummonComponent* SummonComponent = ISummoningActor::GetSummonComponent(Enemy))
	{
		SummonComponent->OnCountChanged.AddUniqueDynamic(this, &ASpawnManager::OnSummonCountChanged);
	}
}

void ASpawnManager::OnSummonCountChanged(const FOnActorTrackerCountChangedPayload& Payload)
{
	if (Payload.IsActorAdded())
	{
		EnemyTrackerComponent->Track(Payload.ChangedActor);
	}
}

void ASpawnManager::OnEnemyCountChanged_Implementation(const FOnActorTrackerCountChangedPayload& Payload)
{
	if (Payload.IsEmpty())
	{
		OnAllSpawnsDestroyed();
		OnAllSpawnsDestroyedDelegate.Broadcast(FAllSpawnsDestroyedPayload(this));
	}
}

void ASpawnManager::OnAllSpawnsDestroyed_Implementation()
{
	// nothing to do here
}

void ASpawnManager::SpawnEnemiesWithDelay_Implementation(const FRandRange& SpawnDelay)
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	if (EnemySpawnTimer.IsValid())
	{
		TimerManager.ClearTimer(EnemySpawnTimer);
	}
	if (Enemies.Num() <= 0)
	{
		return;
	}
	const float SpawnDelayValue = SpawnDelay.Value();
	auto DelayedSpawnLambda = [&, this]()
	{
		if (Enemies.Num() <= 0)
		{
			// this shouldn't trigger, but just in case.
			UE_LOG(LogElectricCastle, Error, TEXT("[%s] Enemies array is empty!"), *GetName())
			return;
		}
		AElectricCastleEnemyCharacter* Enemy = Enemies.Pop();
		FinishSpawningEnemy(Enemy);
		GetWorld()->GetTimerManager().ClearTimer(EnemySpawnTimer);
		if (Enemies.Num() > 0)
		{
			SpawnEnemiesWithDelay(SpawnDelay);
		}
	};
	if (FMath::IsNearlyZero(SpawnDelayValue))
	{
		DelayedSpawnLambda();
	}
	else
	{
		TimerManager.SetTimer(EnemySpawnTimer, DelayedSpawnLambda, SpawnDelayValue, false);
	}
}
