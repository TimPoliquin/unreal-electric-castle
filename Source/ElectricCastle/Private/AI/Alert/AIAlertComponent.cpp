// Copyright Alien Shores


#include "AI/Alert/AIAlertComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "TimerManager.h"
#include "AI/Perception/AIPerceptionManager.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Engine/World.h"
#include "Game/Subsystem/ElectricCastleGameDataSubsystem.h"
#include "Interaction/CombatInterface.h"

UAIAlertComponent::UAIAlertComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	// by default, trigger full alert on damage and touch
	FullAlertStimuli = {EAIPerceptionStimulusType::Damage, EAIPerceptionStimulusType::Touch};
}

void UAIAlertComponent::BeginPlay()
{
	Super::BeginPlay();
	Deactivate();
	if (UElectricCastleGameDataSubsystem* GameDataSubsystem = UElectricCastleGameDataSubsystem::Get(GetOwner()))
	{
		if (GameDataSubsystem->IsGameDataLoaded())
		{
			bProcessAlert = ICombatInterface::IsAlive(GetOwner());
		}
		else
		{
			GameDataSubsystem->OnGameDataLoaded.AddUniqueDynamic(this, &UAIAlertComponent::HandleGameDataLoaded);
		}
	}
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner()))
	{
		CombatInterface->GetOnDeathDelegate().AddUniqueDynamic(this, &UAIAlertComponent::HandleOwnerDeath);
	}
}

void UAIAlertComponent::TickComponent(float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!bProcessAlert)
	{
		return;
	}
	float TickRemainder = DeltaTime;
	if (PerceptionCountdown > 0.f)
	{
		TickRemainder = TickPerceptionCountdown(DeltaTime);
		if (PerceptionCountdown >= 0.f)
		{
			return;
		}
	}
	if (!PerceivedActors.IsEmpty())
	{
		TickIncreaseAlertLevel(TickRemainder);
	}
	else if (!DecayTimer.IsValid() && bAlertLevelDecays)
	{
		TickDecayAlertLevel(TickRemainder);
	}
}

void UAIAlertComponent::BeginDestroy()
{
	Super::BeginDestroy();
	if (UElectricCastleGameDataSubsystem* GameDataSubsystem = UElectricCastleGameDataSubsystem::Get(GetOwner()))
	{
		GameDataSubsystem->OnGameDataLoaded.RemoveAll(this);
	}
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DecayTimer);
		PerceptionCountdown = -1.f;
	}
}

void UAIAlertComponent::InitializePerceptionManager(UAIPerceptionManager* InPerceptionManager)
{
	PerceptionManager = InPerceptionManager;
	PerceptionManager->OnPerceptionAnyStarted.AddUniqueDynamic(this, &UAIAlertComponent::HandlePerceptionAnyStarted);
	PerceptionManager->OnPerceptionAnyEnded.AddUniqueDynamic(this, &UAIAlertComponent::HandlePerceptionAnyEnded);
}

void UAIAlertComponent::OverrideAlertLevel(const EAlertLevel InAlertLevel)
{
	if (!bProcessAlert)
	{
		return;
	}
	if (InAlertLevel == AlertLevel)
	{
		return;
	}
	switch (InAlertLevel)
	{
	case EAlertLevel::Idle:
		SetAlertLevel(0.f);
		break;
	case EAlertLevel::Suspicious:
		Activate();
		SetAlertLevel((SuspiciousThreshold + AlertedThreshold) / 2.f);
		break;
	case EAlertLevel::Alerted:
		Activate();
		SetAlertLevel(AlertedThreshold);
		break;
	default:
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] Attempted to override alert level with unexpected value %s"), *GetOwner()->GetName(), *GetName(),
		       *UEnum::GetValueAsString(InAlertLevel))
		break;
	}
	if (const UWorld* World = GetWorld())
	{
		if (PerceivedActors.IsEmpty() && !DecayTimer.IsValid())
		{
			StartAlertDecayDelayTimer();
		}
		else
		{
			World->GetTimerManager().ClearTimer(DecayTimer);
		}
		PerceptionCountdown = -1.f;
	}
}

void UAIAlertComponent::SetAlertLevelDecays(const bool bInAlertLevelDecays)
{
	bAlertLevelDecays = bInAlertLevelDecays;
}

void UAIAlertComponent::Activate(bool bReset)
{
	Super::Activate(bReset);
	SetComponentTickEnabled(true);
}

void UAIAlertComponent::Deactivate()
{
	if (FMath::IsNearlyZero(AlertLevelRaw))
	{
		Super::Deactivate();
		SetComponentTickEnabled(false);
	}
}

void UAIAlertComponent::HandleGameDataLoaded_Implementation()
{
	bProcessAlert = ICombatInterface::IsAlive(GetOwner());
	if (UElectricCastleGameDataSubsystem* GameDataSubsystem = UElectricCastleGameDataSubsystem::Get(GetOwner()))
	{
		GameDataSubsystem->OnGameDataLoaded.RemoveAll(this);
	}
}

void UAIAlertComponent::HandleOwnerDeath_Implementation(AActor* DeadActor)
{
	bProcessAlert = false;
	AlertLevelRaw = 0.f;
	AlertLevel = EAlertLevel::Idle;
	GetWorld()->GetTimerManager().ClearTimer(DecayTimer);
	PerceptionCountdown = -1.f;
}

void UAIAlertComponent::HandlePerceptionAnyStarted(const FAIPerceptionAnyStartedEventPayload& Payload)
{
	if (Payload.StimulusType == EAIPerceptionStimulusType::Sight)
	{
		if (PerceivedActors.IsEmpty())
		{
			// force perception countdown if this is the first perceived actor
			PerceptionCountdown = FLT_MAX;
		}
		UpdatePerceptionCountdownByActor(Payload.PerceivedActor);
	}
	if (FullAlertStimuli.Contains(Payload.StimulusType) && AlertLevelRaw < AlertedThreshold)
	{
		// trigger full alert level if this is a "full-alert" stimulus 
		SetAlertLevel(AlertedThreshold);
		PerceptionCountdown = -1.f;
	}
	PerceivedActors.AddUnique(Payload.PerceivedActor);
	GetWorld()->GetTimerManager().ClearTimer(DecayTimer);
	Activate();
	if (bDebug)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] Started perceiving actor: %s"), *GetOwner()->GetName(), *GetName(), *Payload.PerceivedActor->GetName())
	}
}

void UAIAlertComponent::HandlePerceptionAnyEnded(const FAIPerceptionAnyEndedEventPayload& Payload)
{
	PerceivedActors.RemoveAll([Payload](const TWeakObjectPtr<AActor> Current)
	{
		return !Current.IsValid() || Current.Get() == Payload.PerceivedActor;
	});
	StartAlertDecayDelayTimer();
}

void UAIAlertComponent::StartAlertDecayDelayTimer()
{
	if (bAlertLevelDecays && PerceivedActors.IsEmpty())
	{
		GetWorld()->GetTimerManager().ClearTimer(DecayTimer);
		GetWorld()->GetTimerManager().SetTimer(
			DecayTimer,
			[this]()
			{
				if (bDebug)
				{
					UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] Begin alert decay"), *GetOwner()->GetName(), *GetName())
				}
				DecayTimer.Invalidate();
			},
			AlertDecayDelay.GetValueAtLevel(AlertLevelRaw),
			false
		);
	}
}

EAlertLevel UAIAlertComponent::GetAlertLevelByValue(const float InAlertLevel) const
{
	if (InAlertLevel >= AlertedThreshold)
	{
		return EAlertLevel::Alerted;
	}
	if (InAlertLevel >= SuspiciousThreshold)
	{
		return EAlertLevel::Suspicious;
	}
	return EAlertLevel::Idle;
}

void UAIAlertComponent::SetAlertLevel(const float InAlertLevel)
{
	if (!bProcessAlert)
	{
		return;
	}
	if (InAlertLevel == AlertLevelRaw)
	{
		return;
	}
	if (bDebug)
	{
		UE_LOG(
			LogElectricCastle,
			Warning,
			TEXT("[%s:%s] Alert level raw changed: %f -> %f"),
			*GetOwner()->GetName(),
			*GetName(),
			AlertLevelRaw,
			InAlertLevel
		)
	}
	const EAlertLevel PreviousAlertLevel = AlertLevel;
	AlertLevelRaw = FMath::Clamp(InAlertLevel, 0.f, AlertedThreshold);
	AlertLevel = GetAlertLevelByValue(InAlertLevel);
	if (PreviousAlertLevel != AlertLevel)
	{
		if (bDebug)
		{
			UE_LOG(
				LogElectricCastle,
				Warning,
				TEXT("[%s:%s] Alert level changed: %s -> %s"),
				*GetOwner()->GetName(),
				*GetName(),
				*UEnum::GetValueAsString(PreviousAlertLevel),
				*UEnum::GetValueAsString(AlertLevel)
			)
		}
		OnAlertLevelChanged.Broadcast(FAlertLevelChangePayload(GetOwner(), PreviousAlertLevel, AlertLevel, AlertLevelRaw));
	}
	OnAlertLevelRawChanged.Broadcast(FAlertLevelChangePayload(GetOwner(), PreviousAlertLevel, AlertLevel, AlertLevelRaw));
}

float UAIAlertComponent::TickPerceptionCountdown(const float DeltaTime)
{
	if (PerceptionCountdown <= 0.f)
	{
		return DeltaTime;
	}
	for (const TWeakObjectPtr<AActor> Actor : PerceivedActors)
	{
		if (!Actor.IsValid())
		{
			continue;
		}
		UpdatePerceptionCountdownByActor(Actor.Get());
	}
	if (DeltaTime >= PerceptionCountdown)
	{
		float Remainder = DeltaTime - PerceptionCountdown;
		PerceptionCountdown = 0;
		return Remainder;
	}
	PerceptionCountdown -= DeltaTime;
	return 0.f;
}

void UAIAlertComponent::TickIncreaseAlertLevel(const float DeltaTime)
{
	if (AlertLevelRaw >= AlertedThreshold)
	{
		// no need to increase alert level - we're already at max
		return;
	}
	// we see a target actor - increase the alert level until it reaches the fully alerted state
	for (const TWeakObjectPtr<AActor> TargetActor : PerceivedActors)
	{
		if (!TargetActor.IsValid())
		{
			continue;
		}
		if (AlertLevelRaw >= AlertedThreshold)
		{
			break;
		}
		const float Distance = FVector::Distance(GetOwner()->GetActorLocation(), TargetActor->GetActorLocation());
		const float Magnitude = PerceptionCurve.GetValueAtLevel(Distance);
		SetAlertLevel(FMath::Clamp(AlertLevelRaw + (Magnitude * DeltaTime), 0.f, AlertedThreshold));
	}
}

void UAIAlertComponent::TickDecayAlertLevel(const float DeltaTime)
{
	// reduce the alert level if alert level decaying is enabled and we're not currently paused
	SetAlertLevel(FMath::Clamp(AlertLevelRaw - (AlertDecayRate.GetValueAtLevel(AlertLevelRaw) * DeltaTime), 0.f, AlertedThreshold));
	if (FMath::IsNearlyZero(AlertLevelRaw))
	{
		SetComponentTickEnabled(false);
		Deactivate();
		if (bDebug)
		{
			UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] Ended alert decay. Sleeping alert component"), *GetOwner()->GetName(), *GetName())
		}
	}
}

void UAIAlertComponent::UpdatePerceptionCountdownByActor(const AActor* Actor)
{
	const float Distance = FVector::Distance(GetOwner()->GetActorLocation(), Actor->GetActorLocation());
	const float NewPerceptionDelay = PerceptionDelayByDistance.GetValueAtLevel(Distance);
	if (NewPerceptionDelay < PerceptionCountdown)
	{
		PerceptionCountdown = NewPerceptionDelay;
	}
}
