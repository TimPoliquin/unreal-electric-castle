// Copyright Alien Shores


#include "Actor/Spawn/EnemySpawnTunnel.h"

#include "Actor/Spawn/ActorTrackerComponent.h"
#include "Actor/Spawn/EnemySpawnConfig.h"
#include "Character/ElectricCastleEnemyCharacter.h"
#include "Components/PointLightComponent.h"
#include "Components/TimelineComponent.h"
#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"
#include "Utils/ArrayUtils.h"
#include "Utils/RandUtils.h"


// Sets default values
AEnemySpawnTunnel::AEnemySpawnTunnel()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	SetRootComponent(StaticMeshComponent);
	SpawnPointComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("SpawnPoint Component"));
	SpawnPointComponent->SetChildActorClass(ATargetPoint::StaticClass());
	SpawnPointComponent->SetupAttachment(GetRootComponent());
	LightComponent = CreateDefaultSubobject<UPointLightComponent>(TEXT("Active Light"));
	LightComponent->SetupAttachment(GetRootComponent());
	EnemyTrackerComponent = CreateDefaultSubobject<UActorTrackerComponent>(TEXT("Enemy Tracker Component"));
	TimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("Timeline"));
	OnLightingTimelineTickDelegate.BindDynamic(this, &AEnemySpawnTunnel::PlayLightingEffect_Tick);
}

// Called when the game starts or when spawned
void AEnemySpawnTunnel::BeginPlay()
{
	Super::BeginPlay();
	LightBaseIntensity = LightComponent->Intensity;
	LightComponent->SetVisibility(false);
	TimelineComponent->AddInterpFloat(LightingEffectCurve, OnLightingTimelineTickDelegate);
	EnemyTrackerComponent->OnCountChanged.AddDynamic(this, &AEnemySpawnTunnel::OnEnemyCountChanged);
}

FOnAuraActorTrackerCountChangedDelegate& AEnemySpawnTunnel::GetOnCountChangedDelegate()
{
	return EnemyTrackerComponent->OnCountChanged;
}

void AEnemySpawnTunnel::BeginSpawning_Implementation(const int32 NumEnemies, const FRandRange& SpawnDelay, const TArray<FEnemySpawnConfig>& SpawnClasses)
{
	PlayLightingEffect();
	PlayStartSound();
	TArray<AElectricCastleEnemyCharacter*> Enemies = InitializeEnemies(NumEnemies, SpawnClasses);
	SpawnEnemiesWithDelay(Enemies, SpawnDelay);
}

TArray<AElectricCastleEnemyCharacter*> AEnemySpawnTunnel::InitializeEnemies(const int32 NumToSpawn, const TArray<FEnemySpawnConfig>& SpawnClasses)
{
	TArray<AElectricCastleEnemyCharacter*> Enemies;
	for (int32 Idx = 0; Idx < NumToSpawn; Idx++)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		const FEnemySpawnConfig& SpawnConfig = UArrayUtils::GetRandomElement(SpawnClasses);
		AElectricCastleEnemyCharacter* Enemy = SpawnPointComponent->GetWorld()->SpawnActorDeferred<AElectricCastleEnemyCharacter>(
			SpawnConfig.EnemyClass,
			SpawnPointComponent->GetChildActor()->GetActorTransform(),
			nullptr,
			nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		);
		Enemy->SetLevel(SpawnConfig.EnemyLevel);
		Enemies.Add(Enemy);
	}
	return Enemies;
}

void AEnemySpawnTunnel::FinishSpawningEnemy(AElectricCastleEnemyCharacter* Enemy)
{
	EnemyTrackerComponent->Track(Enemy);
	Enemy->FinishSpawning(SpawnPointComponent->GetChildActor()->GetActorTransform());
	Enemy->SpawnDefaultController();
}

void AEnemySpawnTunnel::PlayLightingEffect_Implementation()
{
	LightComponent->SetVisibility(true);
	TimelineComponent->PlayFromStart();
}

void AEnemySpawnTunnel::PlayLightingEffect_Tick_Implementation(const float Intensity)
{
	LightComponent->SetIntensity(Intensity * LightBaseIntensity);
}

void AEnemySpawnTunnel::PlayEndSound_Implementation()
{
	UGameplayStatics::PlaySoundAtLocation(this, EndSound, SpawnPointComponent->GetChildActor()->GetActorLocation(), FRotator::ZeroRotator, 1, 1, 0, nullptr, nullptr, this);
}

void AEnemySpawnTunnel::PlayStartSound_Implementation()
{
	UGameplayStatics::PlaySoundAtLocation(this, StartSound, SpawnPointComponent->GetChildActor()->GetActorLocation(), FRotator::ZeroRotator, 1, 1, 0, nullptr, nullptr, this);
}

void AEnemySpawnTunnel::SpawnEnemiesWithDelay_Implementation(TArray<AElectricCastleEnemyCharacter*>& Enemies, const FRandRange& SpawnDelay)
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
	TimerManager.SetTimer(EnemySpawnTimer, [&, this]()
	{
		AElectricCastleEnemyCharacter* Enemy = Enemies.Pop();
		Enemy->SetTargetingRange(100000.f);
		Enemy->SetMaxAIProcessingRange(100000.f);
		FinishSpawningEnemy(Enemy);
		GetWorld()->GetTimerManager().ClearTimer(EnemySpawnTimer);
		if (Enemies.Num() > 0)
		{
			SpawnEnemiesWithDelay(Enemies, SpawnDelay);
		}
	}, SpawnDelay.Value(), false);
}

void AEnemySpawnTunnel::DeactivateSpawnTunnel_Implementation()
{
	LightComponent->SetVisibility(false);
	LightComponent->SetIntensity(0.f);
	PlayEndSound();
}

void AEnemySpawnTunnel::OnEnemyCountChanged(const FOnActorTrackerCountChangedPayload& Payload)
{
	if (Payload.IsEmpty())
	{
		DeactivateSpawnTunnel();
		OnSpawnTunnelDefeated.Broadcast(FOnAuraSpawnTunnelDefeatedPayload(this));
	}
}
