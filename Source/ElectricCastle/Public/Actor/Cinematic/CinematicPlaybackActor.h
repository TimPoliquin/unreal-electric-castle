// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Actor/TriggerInterface.h"
#include "GameFramework/Actor.h"
#include "CinematicPlaybackActor.generated.h"

class UCinematicSequenceMetadata;
class UCinematicContext;

UCLASS()
class ELECTRICCASTLE_API ACinematicPlaybackActor : public AActor, public ITriggerInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACinematicPlaybackActor();
	virtual void PostInitializeComponents() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Start();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void End();
	/** Start ITriggerInterface **/
	virtual bool IsTriggered_Implementation() const override;
	virtual void OnTrigger_Implementation(const bool InActivate) override;
	/** End ITriggerInterface **/

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
