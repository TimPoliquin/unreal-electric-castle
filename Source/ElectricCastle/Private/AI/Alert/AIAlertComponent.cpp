// Copyright Alien Shores


#include "AI/Alert/AIAlertComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AI/Perception/AIPerceptionActor.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Game/Subsystem/ElectricCastleGameDataSubsystem.h"
#include "Interaction/CombatInterface.h"
#include "Perception/AIPerceptionComponent.h"
#include "Tags/ElectricCastleGameplayTags.h"


// Sets default values for this component's properties
UAIAlertComponent::UAIAlertComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAIAlertComponent::BeginPlay()
{
	Super::BeginPlay();
	Deactivate();
	if (UAIPerceptionComponent* InPerceptionComponent = IAIPerceptionActor::GetAIPerceptionComponent(GetOwner()))
	{
		InPerceptionComponent->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &UAIAlertComponent::HandleTargetPerceptionUpdated);
	}
	else
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[%s:%s] Parent should implement IAIPerceptionActor!"), *GetOwner()->GetName(), *GetName())
	}
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
	if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
	{
		AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(FElectricCastleGameplayTags::Get().Event_Alert_Damage).AddUObject(this, &UAIAlertComponent::HandleAlertByDamage);
	}
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner()))
	{
		CombatInterface->GetOnDeathDelegate().AddUniqueDynamic(this, &UAIAlertComponent::HandleOwnerDeath);
	}
}

void UAIAlertComponent::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!bProcessAlert)
	{
		return;
	}
	if (PerceptionCountdown > 0.f)
	{
		PerceptionCountdown = FMath::Max(PerceptionCountdown - DeltaTime, 0.f);
		const float Distance = FVector::Distance(GetOwner()->GetActorLocation(), TargetActor->GetActorLocation());
		const float NewPerceptionDelay = PerceptionDelayByDistance.GetValueAtLevel(Distance);
		if (NewPerceptionDelay < PerceptionCountdown)
		{
			PerceptionCountdown = NewPerceptionDelay;
		}
		if (PerceptionCountdown > 0)
		{
			// end processing during perception countdown
			return;
		}
	}
	if (TargetActor.IsValid() && PerceptionCountdown <= 0.f)
	{
		// we see a target actor - increase the alert level until it reaches the fully alerted state
		if (AlertLevelRaw < AlertedThreshold)
		{
			const float Distance = FVector::Distance(GetOwner()->GetActorLocation(), TargetActor->GetActorLocation());
			const float Magnitude = PerceptionCurve.GetValueAtLevel(Distance);
			LastKnownLocation = TargetActor->GetActorLocation();
			SetAlertLevel(FMath::Clamp(AlertLevelRaw + (Magnitude * DeltaTime), 0.f, AlertedThreshold));
		}
	}
	else if (!DecayTimer.IsValid() && bAlertLevelDecays)
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
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] Attempted to override alert level with unexpected value %s"), *GetOwner()->GetName(), *GetName(), *UEnum::GetValueAsString(InAlertLevel))
		break;
	}
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DecayTimer);
		PerceptionCountdown = -1.f;
	}
}

void UAIAlertComponent::OverrideAlertTarget(AActor* InAlertTarget)
{
	if (!IsValid(InAlertTarget))
	{
		return;
	}
	TargetActor = InAlertTarget;
	LastKnownLocation = InAlertTarget ? InAlertTarget->GetActorLocation() : FVector::ZeroVector;
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

void UAIAlertComponent::HandleAlertByDamage(const FGameplayEventData* GameplayEventData)
{
	if (!bProcessAlert)
	{
		// ignore events after death/before processing is supposed to engage
		return;
	}
	SetAlertLevel(FMath::Clamp(AlertLevelRaw + (AlertedThreshold / 2.f), SuspiciousThreshold, AlertedThreshold));
	if (!TargetActor.IsValid())
	{
		OverrideAlertTarget(GameplayEventData->ContextHandle.GetInstigator());
	}
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DecayTimer);
		PerceptionCountdown = -1.f;
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

void UAIAlertComponent::HandleTargetPerceptionUpdated_Implementation(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		GetWorld()->GetTimerManager().ClearTimer(DecayTimer);
		TargetActor = Actor;
		LastKnownLocation = Stimulus.StimulusLocation;
		PerceptionCountdown = PerceptionDelayByDistance.GetValueAtLevel(FVector::Distance(LastKnownLocation, GetOwner()->GetActorLocation()));
		OnAlertTargetPerceiveChanged.Broadcast(FAlertTargetPerceivedChangePayload(GetOwner(), TargetActor.Get(), LastKnownLocation, true));
		Activate();
		if (bDebug)
		{
			UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] Started perceiving actor: %s"), *GetOwner()->GetName(), *GetName(), *TargetActor->GetName())
		}
	}
	else
	{
		TargetActor = nullptr;
		LastKnownLocation = Stimulus.StimulusLocation;
		PerceptionCountdown = -1.f;
		OnAlertTargetPerceiveChanged.Broadcast(FAlertTargetPerceivedChangePayload(GetOwner(), TargetActor.Get(), LastKnownLocation, false));
		if (bAlertLevelDecays)
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
		OnAlertLevelChanged.Broadcast(FAlertLevelChangePayload(GetOwner(), TargetActor.Get(), LastKnownLocation, PreviousAlertLevel, AlertLevel, AlertLevelRaw));
	}
	OnAlertLevelRawChanged.Broadcast(FAlertLevelChangePayload(GetOwner(), TargetActor.Get(), LastKnownLocation, PreviousAlertLevel, AlertLevel, AlertLevelRaw));
}
