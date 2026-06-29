// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AIPerceptionDataTypes.h"
#include "GameplayEffectTypes.h"
#include "Components/ActorComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "AIPerceptionManager.generated.h"

class UAIPerceptionComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API UAIPerceptionManager : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAIPerceptionManager();
	virtual void InitializePerception(UAIPerceptionComponent* InPerceptionComponent);
	/**
	 * Returns if the actor is actively percieved
	 * @param Actor In actor
	 * @return `true` if actor has at least one active stimulus that can be perceived. `false` otherwise.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsActorPerceived(const AActor* Actor) const;
	// Get Actors that can be actively perceives by at least one stimulus.
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<AActor*> GetCurrentPerceivedActors() const;

	// Broadcast when any actor is perceived. Does not trigger on each new stimulus, only the first.
	UPROPERTY(BlueprintAssignable)
	FAIControllerPerceptionAnyStartedEventSignature OnPerceptionAnyStarted;
	// Broadcast when any actor is no longer perceived. Does not trigger on each stimulus removed, only the last.
	UPROPERTY(BlueprintAssignable)
	FAIControllerPerceptionAnyEndedEventSignature OnPerceptionAnyEnded;
	// Broadcast when the first actor is actively perceived by any stimulus
	UPROPERTY(BlueprintAssignable)
	FAIControllerPerceptionAllStartedEventSignature OnPerceptionAllStarted;
	// Broadcast when the last actor is removed from the perception list
	UPROPERTY(BlueprintAssignable)
	FAIControllerPerceptionAllEndedEventSignature OnPerceptionAllEnded;

private:
	UFUNCTION()
	void HandleOnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	UFUNCTION()
	void HandleTargetPerceptionUpdated_Damage(AActor* Actor, FAIStimulus Stimulus);
	UFUNCTION()
	void HandleTargetPerceptionUpdated_Hearing(AActor* Actor, FAIStimulus Stimulus);
	UFUNCTION()
	void HandleTargetPerceptionUpdated_Sight(AActor* Actor, FAIStimulus Stimulus);
	UFUNCTION()
	void HandleTargetPerceptionUpdated_Touch(AActor* Actor, FAIStimulus Stimulus);
	UFUNCTION()
	void HandleTargetPerceptionUpdated_Other(AActor* Actor, FAIStimulus Stimulus);
	void HandleTargetVisibilityAttributeChanged(const AActor* Actor, const FOnAttributeChangeData& OnAttributeChangeData);

	void AddPerceivedActor(AActor* Actor, const FVector& StimulusLocation, EAIPerceptionStimulusType StimulusType);
	void AddStimulusToActor(const AActor* Actor, EAIPerceptionStimulusType StimulusType);
	void RemoveStimulusFromActor(const AActor* Actor, const EAIPerceptionStimulusType StimulusType);
	void RemoveNoLongerPerceivedActors();
	EAIPerceptionStimulusType DetermineStimulusType(const FAIStimulus& Stimulus) const;
	void NotifyPerceptionUpdates(const EAIPerceptionStimulusType StimulusType);

	UPROPERTY()
	TObjectPtr<UAIPerceptionComponent> PerceptionComponent;
	UPROPERTY()
	TMap<AActor*, FAIPerceivedActor> CurrentPerceivedActors;

	FAISenseID DamageId;
	FAISenseID HearingId;
	FAISenseID SightId;
	FAISenseID TouchId;
};
