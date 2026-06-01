// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "CinematicHandlerTypes.h"
#include "CinematicPlaybackActorInterface.h"
#include "Actor/Trigger/TriggerableInterface.h"
#include "GameFramework/Actor.h"
#include "CinematicPlaybackActor.generated.h"

class UCinematicSequenceMetadata;
class UCinematicContext;

UCLASS()
class ELECTRICCASTLE_API ACinematicPlaybackActor : public AActor, public ICinematicPlaybackActorInterface, public ITriggerableInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACinematicPlaybackActor();
	virtual void PostInitializeComponents() override;

	/** Start ICinematicPlaybackActorInterface **/
	virtual void Start_Implementation() override;
	virtual void End_Implementation() override;
	virtual FCinematicPlaybackEventSignature& GetOnPlaybackStartDelegate() override { return OnCinematicPlaybackStart; }
	virtual FCinematicPlaybackEventSignature& GetOnPlaybackEndDelegate() override { return OnCinematicPlaybackEnd; }
	/** End ICinematicPlaybackActorInterface **/

	/** Start ITriggerInterface **/
	virtual bool IsTriggered_Implementation() const override;
	virtual void OnTrigger_Implementation(const bool InActivate) override;
	/** End ITriggerInterface **/

	FCinematicPlaybackEventSignature OnCinematicPlaybackStart;
	FCinematicPlaybackEventSignature OnCinematicPlaybackEnd;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category="Properties")
	TObjectPtr<UCinematicSequenceMetadata> Metadata;

	UPROPERTY(BlueprintReadOnly)
	bool bIsPlaying = false;
	UPROPERTY(BlueprintReadOnly, SaveGame)
	bool bHasPlayed = false;

private:
	UPROPERTY()
	TObjectPtr<UCinematicContext> CinematicContext;
};
