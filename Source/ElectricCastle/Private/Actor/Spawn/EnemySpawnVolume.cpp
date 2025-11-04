// Copyright Alien Shores


#include "Actor/Spawn/EnemySpawnVolume.h"

#include "Actor/Spawn/ActorTrackerComponent.h"
#include "Actor/Spawn/EnemySpawnPoint.h"
#include "Character/ElectricCastleEnemyCharacter.h"
#include "Components/BoxComponent.h"
#include "Interaction/PlayerInterface.h"


// Sets default values
AEnemySpawnVolume::AEnemySpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	SetRootComponent(Box);
	Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Box->SetCollisionResponseToAllChannels(ECR_Ignore);
	Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	ActorTrackerComponent = CreateDefaultSubobject<UActorTrackerComponent>(TEXT("Actor Tracker Component"));
}

void AEnemySpawnVolume::SetEnabled(const bool bInEnabled)
{
	bEnabled = bInEnabled;
	SetActorHiddenInGame(!bInEnabled);
	SetActorEnableCollision(bInEnabled);
	if (!bInEnabled)
	{
		SetActorTickEnabled(false);
	}
}

void AEnemySpawnVolume::PostLoad_Implementation()
{
	if (bTriggered)
	{
		SetEnabled(false);
	}
}

void AEnemySpawnVolume::BeginPlay()
{
	Super::BeginPlay();
	if (!bEnabled)
	{
		SetEnabled(false);
	}
	Box->OnComponentBeginOverlap.AddDynamic(this, &AEnemySpawnVolume::OnBoxOverlap);
	ActorTrackerComponent->OnCountChanged.AddDynamic(this, &AEnemySpawnVolume::OnActorTrackerCountChangedHandler);
}

void AEnemySpawnVolume::OnBoxOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (!IPlayerInterface::ImplementsPlayerInterface(OtherActor) || !bEnabled)
	{
		return;
	}
	bTriggered = true;
	AddChildSpawnPoints();
	for (const AEnemySpawnPoint* SpawnPoint : SpawnPoints)
	{
		if (IsValid(SpawnPoint))
		{
			ActorTrackerComponent->Track(SpawnPoint->SpawnEnemy());
		}
	}
	SetEnabled(false);
}

void AEnemySpawnVolume::OnActorTrackerCountChangedHandler(const FOnActorTrackerCountChangedPayload& Payload)
{
	if (Payload.IsEmpty())
	{
		OnAllEnemiesDefeated.Broadcast();
	}
}

void AEnemySpawnVolume::AddChildSpawnPoints()
{
	TArray<AActor*> ChildActors;
	GetAttachedActors(ChildActors, true, true);
	for (AActor* Child : ChildActors)
	{
		if (IsValid(Child) && Child->IsA(AEnemySpawnPoint::StaticClass()))
		{
			SpawnPoints.AddUnique(Cast<AEnemySpawnPoint>(Child));
		}
	}
}
