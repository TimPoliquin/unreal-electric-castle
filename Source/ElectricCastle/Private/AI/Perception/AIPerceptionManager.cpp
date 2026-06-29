// Copyright Alien Shores


#include "AI/Perception/AIPerceptionManager.h"

#include "GameplayEffectExtension.h"
#include "AbilitySystem/ElectricCastleAbilitySystemComponent.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "AbilitySystem/ElectricCastleAttributeSet.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Touch.h"

UAIPerceptionManager::UAIPerceptionManager()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UAIPerceptionManager::InitializePerception(UAIPerceptionComponent* InPerceptionComponent)
{
	PerceptionComponent = InPerceptionComponent;
	PerceptionComponent->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &UAIPerceptionManager::HandleOnTargetPerceptionUpdated);
	DamageId = UAISense::GetSenseID<UAISense_Damage>();
	HearingId = UAISense::GetSenseID<UAISense_Hearing>();
	SightId = UAISense::GetSenseID<UAISense_Sight>();
	TouchId = UAISense::GetSenseID<UAISense_Touch>();
}

bool UAIPerceptionManager::IsActorPerceived(const AActor* Actor) const
{
	if (!CurrentPerceivedActors.Contains(Actor))
	{
		return false;
	}
	const FAIPerceivedActor& PerceivedActor = CurrentPerceivedActors[Actor];
	return PerceivedActor.IsValid() && PerceivedActor.IsPerceived();
}

TArray<AActor*> UAIPerceptionManager::GetCurrentPerceivedActors() const
{
	TArray<AActor*> ActiveActors;
	for (const TTuple<AActor*, FAIPerceivedActor>& PerceivedActorTuple : CurrentPerceivedActors)
	{
		if (PerceivedActorTuple.Value.IsPerceived())
		{
			ActiveActors.Add(PerceivedActorTuple.Key);
		}
	}
	return ActiveActors;
}

void UAIPerceptionManager::HandleOnTargetPerceptionUpdated(AActor* Actor, const FAIStimulus Stimulus)
{
	const EAIPerceptionStimulusType StimulusType = DetermineStimulusType(Stimulus);
	// add/update/remove the stimulus. add a new PerceivedActor if this is the first stimulus for that actor. remove the PerceivedActor if this is the last stimulus.
	if (!Stimulus.WasSuccessfullySensed())
	{
		RemoveStimulusFromActor(Actor, StimulusType);
	}
	else if (!CurrentPerceivedActors.Contains(Actor))
	{
		AddPerceivedActor(Actor, Stimulus.StimulusLocation, StimulusType);
	}
	else
	{
		AddStimulusToActor(Actor, StimulusType);
	}

	// Perform sense-specific actions (add event listeners, etc).
	switch (StimulusType)
	{
	case EAIPerceptionStimulusType::Sight:
		HandleTargetPerceptionUpdated_Sight(Actor, Stimulus);
		break;
	case EAIPerceptionStimulusType::Damage:
		HandleTargetPerceptionUpdated_Damage(Actor, Stimulus);
		break;
	case EAIPerceptionStimulusType::Hearing:
		HandleTargetPerceptionUpdated_Hearing(Actor, Stimulus);
		break;
	case EAIPerceptionStimulusType::Touch:
		HandleTargetPerceptionUpdated_Touch(Actor, Stimulus);
		break;
	default:
		HandleTargetPerceptionUpdated_Other(Actor, Stimulus);
	}

	// dispatch events based on actor perception status
	NotifyPerceptionUpdates(StimulusType);
}

void UAIPerceptionManager::HandleTargetPerceptionUpdated_Damage(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Stimulus.WasSuccessfullySensed())
	{
	}
	// what do we want to do when the pawn is damaged?
	// if the pawn is idle, maybe this should trigger surprise/stagger?
	// if the pawn is suspicious, this should trigger full-on combat
	// if the pawn is actively engaged in combat, it probably doesn't matter.
}

void UAIPerceptionManager::HandleTargetPerceptionUpdated_Hearing(AActor* Actor, FAIStimulus Stimulus)
{
}

void UAIPerceptionManager::HandleTargetPerceptionUpdated_Sight(AActor* Actor, FAIStimulus Stimulus)
{
	if (CurrentPerceivedActors.Contains(Actor))
	{
		FAIPerceivedActor& PerceivedActor = CurrentPerceivedActors[Actor];
		if (!Stimulus.WasSuccessfullySensed())
		{
			// remove the delegate
			PerceivedActor.RemoveVisibilityChangedDelegate();
		}
		else
		{
			PerceivedActor.RemoveVisibilityChangedDelegate();
			if (UElectricCastleAbilitySystemComponent* AbilitySystemComponent = UElectricCastleAbilitySystemLibrary::GetAbilitySystemComponent(Actor))
			{
				PerceivedActor.VisibilityChangedDelegateHandle =
					AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
						                      UElectricCastleAttributeSet::GetVisibilityAttribute())
					                      .AddLambda([Actor, this](const FOnAttributeChangeData& Payload)
					                      {
						                      HandleTargetVisibilityAttributeChanged(Actor, Payload);
					                      });
			}
		}
	}
}

void UAIPerceptionManager::HandleTargetPerceptionUpdated_Touch(AActor* Actor, FAIStimulus Stimulus)
{
}

void UAIPerceptionManager::HandleTargetPerceptionUpdated_Other(AActor* Actor, FAIStimulus Stimulus)
{
}

void UAIPerceptionManager::HandleTargetVisibilityAttributeChanged(const AActor* Actor, const FOnAttributeChangeData& OnAttributeChangeData)
{
	if (CurrentPerceivedActors.Contains(Actor))
	{
		FAIPerceivedActor& PerceivedActor = CurrentPerceivedActors[Actor];
		PerceivedActor.bIsVisible = UElectricCastleAbilitySystemLibrary::GetVisibilityAttributeValue(Actor) > 0.f;
		NotifyPerceptionUpdates(EAIPerceptionStimulusType::Sight);
	}
}

void UAIPerceptionManager::AddPerceivedActor(AActor* Actor, const FVector& StimulusLocation, const EAIPerceptionStimulusType StimulusType)
{
	if (CurrentPerceivedActors.Contains(Actor))
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[%s:%s] Actor %s is already tracked as a perceived actor!"), *GetOwner()->GetName(), *GetName(), *Actor->GetName());
		return;
	}
	FAIPerceivedActor PerceivedActor;
	PerceivedActor.PerceivedActor = Actor;
	PerceivedActor.LastKnownLocation = Actor->GetActorLocation();
	PerceivedActor.ActiveStimuli = {StimulusType};
	PerceivedActor.bIsVisible = UElectricCastleAbilitySystemLibrary::GetVisibilityAttributeValue(Actor) > 0.f;
	CurrentPerceivedActors.Add(Actor, PerceivedActor);
}

void UAIPerceptionManager::AddStimulusToActor(const AActor* Actor, const EAIPerceptionStimulusType StimulusType)
{
	if (!CurrentPerceivedActors.Contains(Actor))
	{
		// nothing to do here - but it's weird!
		UE_LOG(
			LogElectricCastle,
			Warning,
			TEXT("[%s:%s] Attempted to add stimulus %s to actor %s, but they are not currently tracked!"),
			*GetOwner()->GetName(),
			*GetName(),
			*UEnum::GetValueAsString(StimulusType),
			*Actor->GetName()
		);
		return;
	}
	FAIPerceivedActor& PerceivedActor = CurrentPerceivedActors[Actor];
	PerceivedActor.ActiveStimuli.Add(StimulusType);
	PerceivedActor.bIsVisible = UElectricCastleAbilitySystemLibrary::GetVisibilityAttributeValue(Actor) > 0.f;
}

void UAIPerceptionManager::RemoveStimulusFromActor(const AActor* Actor, const EAIPerceptionStimulusType StimulusType)
{
	if (!CurrentPerceivedActors.Contains(Actor))
	{
		// nothing to do here - but it's weird!
		UE_LOG(
			LogElectricCastle,
			Warning,
			TEXT("[%s:%s] Attempted to remove stimulus %s for actor %s, but they are not currently tracked!"),
			*GetOwner()->GetName(),
			*GetName(),
			*UEnum::GetValueAsString(StimulusType),
			*Actor->GetName()
		);
		return;
	}
	FAIPerceivedActor& PerceivedActor = CurrentPerceivedActors[Actor];
	PerceivedActor.ActiveStimuli.Remove(StimulusType);
	PerceivedActor.bIsVisible = UElectricCastleAbilitySystemLibrary::GetVisibilityAttributeValue(Actor) > 0.f;
}

void UAIPerceptionManager::RemoveNoLongerPerceivedActors()
{
	if (!PerceptionComponent)
	{
		return;
	}
	TArray<AActor*> PerceivedActors;
	TArray<AActor*> ToRemove;
	PerceptionComponent->GetCurrentlyPerceivedActors(nullptr, PerceivedActors);
	for (const TTuple<AActor*, FAIPerceivedActor>& PerceivedActorPair : CurrentPerceivedActors)
	{
		if (!PerceivedActorPair.Value.IsValid() || !PerceivedActors.Contains(PerceivedActorPair.Key))
		{
			ToRemove.Add(PerceivedActorPair.Key);
		}
	}
	for (const AActor* RemoveActor : ToRemove)
	{
		CurrentPerceivedActors.Remove(RemoveActor);
	}
}

EAIPerceptionStimulusType UAIPerceptionManager::DetermineStimulusType(const FAIStimulus& Stimulus) const
{
	if (Stimulus.Type == SightId)
	{
		return EAIPerceptionStimulusType::Sight;
	}
	if (Stimulus.Type == HearingId)
	{
		return EAIPerceptionStimulusType::Hearing;
	}
	if (Stimulus.Type == DamageId)
	{
		return EAIPerceptionStimulusType::Damage;
	}
	if (Stimulus.Type == TouchId)
	{
		return EAIPerceptionStimulusType::Touch;
	}
	return EAIPerceptionStimulusType::Unknown;
}

void UAIPerceptionManager::NotifyPerceptionUpdates(const EAIPerceptionStimulusType StimulusType)
{
	bool bShouldNotifyAllStarted = true;
	bool bShouldNotifyAllEnded = true;
	TArray<FAIPerceivedActor> NotifyPerceivedActors;
	TArray<FAIPerceivedActor> NotifyUnperceivedActors;
	if (CurrentPerceivedActors.IsEmpty())
	{
		bShouldNotifyAllStarted = false;
	}
	else
	{
		for (const TTuple<AActor*, FAIPerceivedActor>& ActorPair : CurrentPerceivedActors)
		{
			const FAIPerceivedActor& PerceivedActor = ActorPair.Value;
			if (PerceivedActor.IsPerceived())
			{
				// some are still being perceived, so do not broadcast the all finished event
				if (bShouldNotifyAllEnded)
				{
					bShouldNotifyAllEnded = false;
				}
				if (!PerceivedActor.bHasNotifiedOnPerceived)
				{
					// queue this perceived actor for notification
					NotifyPerceivedActors.Add(PerceivedActor);
				}
				else if (bShouldNotifyAllStarted)
				{
					// if this actor has already notified that it has been perceived, do not perform the all started notify. 
					bShouldNotifyAllStarted = false;
				}
			}
			else
			{
				if (!PerceivedActor.bHasNotifiedOnUnperceived)
				{
					NotifyUnperceivedActors.Add(PerceivedActor);
				}
			}
		}
	}
	if (bShouldNotifyAllStarted)
	{
		OnPerceptionAllStarted.Broadcast(FAIPerceptionAllStartedEventPayload());
	}
	for (FAIPerceivedActor& PerceivedActor : NotifyPerceivedActors)
	{
		OnPerceptionAnyStarted.Broadcast(FAIPerceptionAnyStartedEventPayload(PerceivedActor.PerceivedActor.Get(), StimulusType, PerceivedActor.LastKnownLocation));
		PerceivedActor.bHasNotifiedOnPerceived = true;
	}
	for (FAIPerceivedActor& PerceivedActor : NotifyUnperceivedActors)
	{
		OnPerceptionAnyEnded.Broadcast(FAIPerceptionAnyEndedEventPayload(PerceivedActor.PerceivedActor.Get()));
		PerceivedActor.bHasNotifiedOnUnperceived = true;
		if (!PerceivedActor.HasAnyStimuli())
		{
			CurrentPerceivedActors.Remove(PerceivedActor.PerceivedActor.Get());
		}
	}
	if (bShouldNotifyAllEnded)
	{
		OnPerceptionAllEnded.Broadcast(FAIPerceptionAllEndedEventPayload());
	}
}
