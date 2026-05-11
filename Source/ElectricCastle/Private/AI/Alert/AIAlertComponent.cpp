// Copyright Alien Shores


#include "AI/Alert/AIAlertComponent.h"

#include "AI/Perception/AIPerceptionActor.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Game/Subsystem/ElectricCastleGameDataSubsystem.h"
#include "Perception/AIPerceptionComponent.h"


// Sets default values for this component's properties
UAIAlertComponent::UAIAlertComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAIAlertComponent::BeginPlay()
{
	Super::BeginPlay();
	SetComponentTickEnabled(false);
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
			bProcessAlert = true;
		}
		else
		{
			GameDataSubsystem->OnGameDataLoaded.AddUniqueDynamic(this, &UAIAlertComponent::HandleGameDataLoaded);
		}
	}
}

void UAIAlertComponent::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!bProcessAlert)
	{
		return;
	}
	if (TargetActor.IsValid())
	{
		if (AlertLevelRaw < AlertedThreshold)
		{
			const float Distance = FVector::Distance(GetOwner()->GetActorLocation(), TargetActor->GetActorLocation());
			const float Magnitude = PerceptionCurve.GetValueAtLevel(Distance);
			SetAlertLevel(FMath::Clamp(AlertLevelRaw + (Magnitude * DeltaTime), 0.f, AlertedThreshold));
			LastKnownLocation = TargetActor->GetActorLocation();
		}
	}
	else if (!DecayTimer.IsValid())
	{
		SetAlertLevel(FMath::Clamp(AlertLevelRaw - (AlertDecayRate.GetValueAtLevel(AlertLevelRaw) * DeltaTime), 0.f, AlertedThreshold));
		if (FMath::IsNearlyZero(AlertLevelRaw))
		{
			SetComponentTickEnabled(false);
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
}

void UAIAlertComponent::HandleGameDataLoaded_Implementation()
{
	bProcessAlert = true;
	if (UElectricCastleGameDataSubsystem* GameDataSubsystem = UElectricCastleGameDataSubsystem::Get(GetOwner()))
	{
		GameDataSubsystem->OnGameDataLoaded.RemoveAll(this);
	}
}

void UAIAlertComponent::HandleTargetPerceptionUpdated_Implementation(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		TargetActor = Actor;
		LastKnownLocation = Stimulus.StimulusLocation;
		OnAlertTargetPerceiveChanged.Broadcast(FAlertTargetPerceivedChangePayload(GetOwner(), TargetActor.Get(), LastKnownLocation, true));
		SetComponentTickEnabled(true);
		if (bDebug)
		{
			UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] Started perceiving actor: %s"), *GetOwner()->GetName(), *GetName(), *Actor->GetName())
		}
		GetWorld()->GetTimerManager().ClearTimer(DecayTimer);
	}
	else
	{
		TargetActor = nullptr;
		LastKnownLocation = Stimulus.StimulusLocation;
		OnAlertTargetPerceiveChanged.Broadcast(FAlertTargetPerceivedChangePayload(GetOwner(), TargetActor.Get(), LastKnownLocation, false));
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
	if (InAlertLevel == AlertLevelRaw)
	{
		return;
	}
	const EAlertLevel NewAlertLevel = GetAlertLevelByValue(InAlertLevel);
	if (AlertLevel != NewAlertLevel)
	{
		OnAlertLevelChanged.Broadcast(FAlertLevelChangePayload(GetOwner(), TargetActor.Get(), LastKnownLocation, AlertLevel, NewAlertLevel, InAlertLevel));
		if (bDebug)
		{
			UE_LOG(
				LogElectricCastle,
				Warning,
				TEXT("[%s:%s] Alert level changed: %s -> %s"),
				*GetOwner()->GetName(),
				*GetName(),
				*UEnum::GetValueAsString(AlertLevel),
				*UEnum::GetValueAsString(NewAlertLevel)
			)
		}
	}
	else if (bDebug)
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
	OnAlertLevelRawChanged.Broadcast(FAlertLevelChangePayload(GetOwner(), TargetActor.Get(), LastKnownLocation, AlertLevel, NewAlertLevel, InAlertLevel));
	AlertLevelRaw = InAlertLevel;
	AlertLevel = NewAlertLevel;
}
