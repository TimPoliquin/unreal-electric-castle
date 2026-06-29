// Copyright Alien Shores


#include "Actor/Encounter/EncounterManager.h"

#include "TimerManager.h"
#include "AI/ElectricCastleAIController.h"
#include "AI/Alert/AIAlertActor.h"
#include "AI/Alert/AIAlertComponent.h"
#include "AI/Engagement/AIEngagementActor.h"
#include "AI/Engagement/AIEngagementController.h"
#include "AI/Targeting/AITargetingActorInterface.h"
#include "AI/Targeting/AITargetingComponent.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "Actor/Encounter/EncounterBlueprintFunctionLibrary.h"
#include "Actor/Significance/Component/ActorSignificanceComponent.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Game/Subsystem/ElectricCastleGameDataSubsystem.h"
#include "Interaction/CombatInterface.h"
#include "Tags/ElectricCastleGameplayTags.h"
#include "Utils/ArrayUtils.h"

AEncounterManager::AEncounterManager()
{
	PrimaryActorTick.bCanEverTick = false;
	SignificanceComponent = CreateDefaultSubobject<UActorSignificanceComponent>(TEXT("Significance Component"));
	SignificanceComponent->SetSignificanceTag(FElectricCastleGameplayTags::Get().Significance_Category_Encounter);
}


void AEncounterManager::RegisterTarget_Implementation(AActor* Target)
{
	if (IsValid(Target))
	{
		EncounterTargets.AddUnique(Target);
	}
}

void AEncounterManager::UnregisterTarget_Implementation(AActor* Target)
{
	if (IsValid(Target))
	{
		EncounterTargets.RemoveAll(
			[Target](const TWeakObjectPtr<AActor> Current)
			{
				return !Current.IsValid() || Current.Get() == Target;
			}
		);
	}
}

void AEncounterManager::RegisterEnemy_Implementation(AActor* Enemy)
{
	if (!ICombatInterface::IsAlive(Enemy))
	{
		return;
	}
	if (bDebug)
	{
		UE_LOG(LogElectricCastle, Display, TEXT("[%s] Adding enemy to encounter: %s"), *GetName(), *Enemy->GetName());
	}
	Enemies.AddUnique(Enemy);
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Enemy))
	{
		CombatInterface->GetOnDeathDelegate().AddUniqueDynamic(this, &AEncounterManager::HandleEnemyDeath);
	}
	if (UAIAlertComponent* AlertComponent = IAIAlertActor::GetAIAlertComponent(Enemy))
	{
		AlertComponent->OnAlertLevelChanged.AddUniqueDynamic(this, &AEncounterManager::HandleEnemyAlertLevelChanged);
	}
}

void AEncounterManager::EnterSignificance_FullySignificant_Implementation()
{
	if (EncounterAlertLevel == EAlertLevel::Alerted)
	{
		RecalibrateEncounter();
	}
}

void AEncounterManager::EnterSignificance_PartiallySignificant_Implementation()
{
	GetWorld()->GetTimerManager().ClearTimer(RecalibrationTimerHandle);
}

void AEncounterManager::EnterSignificance_Insignificant_Implementation()
{
	GetWorld()->GetTimerManager().ClearTimer(RecalibrationTimerHandle);
}

void AEncounterManager::BeginPlay()
{
	Super::BeginPlay();
	if (UElectricCastleGameDataSubsystem* GameDataSubsystem = UElectricCastleGameDataSubsystem::Get(this))
	{
		if (GameDataSubsystem->IsGameDataLoaded())
		{
			HandleGameDataLoaded();
		}
		else
		{
			GameDataSubsystem->OnGameDataLoaded.AddUniqueDynamic(this, &AEncounterManager::HandleGameDataLoaded);
		}
	}
}

TArray<AActor*> AEncounterManager::GetValidEncounterTargets() const
{
	TArray<AActor*> ValidTargets;
	if (!EncounterTargets.IsEmpty())
	{
		// get visible targets in the encounter
		for (const TWeakObjectPtr<AActor>& EncounterTarget : EncounterTargets)
		{
			if (!EncounterTarget.IsValid())
			{
				continue;
			}
			// add targets that are visible
			if (UElectricCastleAbilitySystemLibrary::GetVisibilityAttributeValue(EncounterTarget.Get()) > 0.f)
			{
				ValidTargets.Add(EncounterTarget.Get());
			}
		}
	}
	if (ValidTargets.IsEmpty())
	{
		// if no targets are available, try to use enemy perceptions to decide on a target
		ValidTargets = UEncounterBlueprintFunctionLibrary::GetAllActorsPerceivedByWeakActors(Enemies);
	}
	return ValidTargets;
}

void AEncounterManager::HandleGameDataLoaded_Implementation()
{
	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors);
	for (AActor* Actor : AttachedActors)
	{
		RegisterEnemy(Actor);
	}
}

void AEncounterManager::HandleEnemyAlertLevelChanged_Implementation(const FAlertLevelChangePayload& Payload)
{
	if (Payload.NewAlertLevel == EncounterAlertLevel)
	{
		return;
	}
	switch (Payload.NewAlertLevel)
	{
	case EAlertLevel::Alerted:
		EncounterAlertLevel = Payload.NewAlertLevel;
		AlertAllEnemiesInEncounter(Payload);
		RecalibrateEncounter();
		break;
	case EAlertLevel::Suspicious:
		EncounterAlertLevel = Payload.NewAlertLevel;
		break;
	case EAlertLevel::Idle:
		// if all of the enemies are idle, mark the engagement as no longer triggered
		if (!Enemies.ContainsByPredicate(
			[](const TWeakObjectPtr<AActor> Actor)
			{
				if (!Actor.IsValid() || ICombatInterface::IsDead(Actor.Get()))
				{
					return false;
				}
				if (const UAIAlertComponent* AlertComponent = IAIAlertActor::GetAIAlertComponent(Actor.Get()))
				{
					return AlertComponent->GetCurrentAlertLevel() > EAlertLevel::Idle;
				}
				return false;
			}
		))
		{
			EncounterAlertLevel = Payload.NewAlertLevel;
		}
		break;
	default:
		break;
	}
}

void AEncounterManager::AlertAllEnemiesInEncounter_Implementation(const FAlertLevelChangePayload& Payload)
{
	for (TWeakObjectPtr<AActor> Enemy : Enemies)
	{
		if (!Enemy.IsValid() || ICombatInterface::IsDead(Enemy.Get()))
		{
			continue;
		}
		if (Enemy.Get() == Payload.Owner)
		{
			continue;
		}
		if (UAIAlertComponent* EnemyAlertComponent = IAIAlertActor::GetAIAlertComponent(Enemy.Get()))
		{
			EnemyAlertComponent->OverrideAlertLevel(EAlertLevel::Alerted);
		}
	}
}

void AEncounterManager::HandleEnemyDeath_Implementation(AActor* DeadActor)
{
	Enemies.RemoveAll(
		[DeadActor](const TWeakObjectPtr<AActor> Actor)
		{
			return !Actor.IsValid() || Actor.Get() == DeadActor || ICombatInterface::IsDead(Actor.Get());
		}
	);
	RecalibrateEncounter();
}

void AEncounterManager::RecalibrateEncounter_Implementation()
{
	if (bDebug)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Recalibrating encounter"), *GetName())
	}
	GetWorld()->GetTimerManager().ClearTimer(RecalibrationTimerHandle);
	// Cleanup stale actors
	Enemies.RemoveAll(
		[](const TWeakObjectPtr<AActor>& Actor)
		{
			return !Actor.IsValid() || ICombatInterface::IsDead(Actor.Get());
		}
	);
	if (Enemies.IsEmpty())
	{
		return;
	}
	const TArray<AActor*>& AllPerceivedTargetActors = GetValidEncounterTargets();
	// copy the list
	TArray<TWeakObjectPtr<AActor>> AllEncounterEnemies = Enemies;
	UArrayUtils::ShuffleArray(AllEncounterEnemies);
	// pick the actors that will be aggressive
	TArray<UAIEngagementController*> PassiveControllers;
	const int32 PreferredAggressiveActorCount = FMath::Min(URandRangeBlueprintLibrary::GetRandomInt32InRange(EngagementConfig.PreferredCount), AllEncounterEnemies.Num());
	int32 AggressiveActorCount = 0;
	for (int32 Idx = 0; Idx < AllEncounterEnemies.Num(); Idx++)
	{
		AActor* Actor = AllEncounterEnemies[Idx].Get();
		if (UAIEngagementController* EngagementController = IAIEngagementActor::GetAIEngagementController(Actor))
		{
			if (EngagementController->GetCurrentEngagementControlMode() != EEngagementControlMode::Directed)
			{
				continue;
			}
			if (AggressiveActorCount < PreferredAggressiveActorCount && EngagementController->DoesPreferAggressiveEngagement())
			{
				MakeEnemyAggressive(Actor, EngagementController);
				AggressiveActorCount++;
			}
			else
			{
				PassiveControllers.Add(EngagementController);
			}
		}
		if (UAITargetingComponent* TargetingController = IAITargetingActorInterface::GetAITargetingComponent(Actor))
		{
			if (!AllPerceivedTargetActors.IsEmpty())
			{
				TargetingController->SetCurrentTarget(UArrayUtils::GetRandomElement(AllPerceivedTargetActors));
			}
			else
			{
				TargetingController->SetCurrentTarget(nullptr);
			}
		}
	}

	for (UAIEngagementController* EngagementController : PassiveControllers)
	{
		// if not enough aggressive actors were found, make somebody else aggressive!
		if (AggressiveActorCount < PreferredAggressiveActorCount)
		{
			MakeEnemyAggressive(EngagementController->GetOwner(), EngagementController);
			AggressiveActorCount++;
		}
		else
		{
			// otherwise, let them be passive
			MakeEnemyPassive(EngagementController->GetOwner(), EngagementController);
		}
	}
	// refresh aggressive actors on a timer
	GetWorld()->GetTimerManager().SetTimer(
		RecalibrationTimerHandle,
		[this]()
		{
			RecalibrateEncounter();
		},
		URandRangeBlueprintLibrary::GetRandomFloatInRange(RecalibrationTimeRange),
		false
	);
}

void AEncounterManager::MakeEnemyAggressive_Implementation(AActor* Actor, UAIEngagementController* EngagementController)
{
	if (bDebug)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Making enemy aggressive: %s"), *GetName(), *Actor->GetName())
	}
	EngagementController->SetCurrentEngagementLevel(EngagementController->GetRandomAggressiveEngagementLevel());
	EngagementController->SetCurrentEngagementRange(EngagementController->GetPreferredEngagementRange());
	EngagementController->ChangeItUp();
}

void AEncounterManager::MakeEnemyPassive_Implementation(AActor* Actor, UAIEngagementController* EngagementController)
{
	if (bDebug)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Making enemy passive: %s"), *GetName(), *Actor->GetName())
	}
	EngagementController->SetCurrentEngagementLevel(EngagementController->GetRandomPassiveEngagementLevel(EngagementConfig.bAllowNoneEngagement));
	EngagementController->SetCurrentEngagementRange(EngagementController->GetRandomEngagementRange());
	EngagementController->ChangeItUp();
}
