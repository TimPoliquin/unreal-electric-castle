// Copyright Alien Shores


#include "Actor/Spawn/EnemySpawnTunnel.h"

#include "Actor/Spawn/ActorTrackerComponent.h"
#include "Actor/Spawn/EnemySpawnConfig.h"
#include "Character/ElectricCastleEnemyCharacter.h"
#include "Components/PointLightComponent.h"
#include "Components/TimelineComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AEnemySpawnTunnel::AEnemySpawnTunnel()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	StaticMeshComponent->SetupAttachment(GetRootComponent());
	LightComponent = CreateDefaultSubobject<UPointLightComponent>(TEXT("Active Light"));
	LightComponent->SetupAttachment(GetRootComponent());
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
}

void AEnemySpawnTunnel::OnAllSpawnsDestroyed_Implementation()
{
	DeactivateSpawnTunnel();
}

void AEnemySpawnTunnel::FinishSpawningEnemy(AElectricCastleEnemyCharacter* Enemy)
{
	Super::FinishSpawningEnemy(Enemy);
	Enemy->SetTargetingRange(100000.f);
	Enemy->SetMaxAIProcessingRange(100000.f);
}

void AEnemySpawnTunnel::BeginSpawning_Implementation(const int32 NumEnemies, const FRandRange& SpawnDelay, const TArray<FEnemySpawnConfig>& SpawnClasses)
{
	PlayLightingEffect();
	PlayStartSound();
	Super::BeginSpawning_Implementation(NumEnemies, SpawnDelay, SpawnClasses);
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

void AEnemySpawnTunnel::DeactivateSpawnTunnel_Implementation()
{
	LightComponent->SetVisibility(false);
	LightComponent->SetIntensity(0.f);
	PlayEndSound();
}
