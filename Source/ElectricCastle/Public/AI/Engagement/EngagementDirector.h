// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EngagementDirector.generated.h"

UCLASS()
class ELECTRICCASTLE_API AEngagementDirector : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEngagementDirector();

	/**
	 * Recalculates the engagement values for engagement participants. May not fully process all participants. 
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void RecalculateEngagement();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void AddParticipant(AActor* Participant);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void RemoveParticipant(AActor* Participant);


	// TODO - create a class EngagementDirector
	// EngagementDirector will have lists of FEngagementAIConfig for each Engagement Level
	// EngagementDirector will need to be able to find/detect all potential participants within range
	//		Enemy team members that are nearby but haven't been alerted yet
	//		Player character
	//		Player summons/allies
	// EngagementDirector will definitely be responsible for directing enemy engagement values
	// EngagementDirector may not be responsible for directing summons/ally engagement values
	// Enemies/Summons/Allies will need a new component to manage engagement ranges/engagement ability delays
	// Abilities will need configuration to identify whether they are ranged/close and support/defensive/offensive
};
