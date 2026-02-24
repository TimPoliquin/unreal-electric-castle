// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "CinematicEvents.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CinematicManager.generated.h"

class UCinematicContextHandle;
class ULevelSequencePlayer;
/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UCinematicManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static UCinematicManager* Get(const UObject* WorldContextObject);

	void RegisterSequencePlayer(ULevelSequencePlayer* LevelSequencePlayer, ULevelSequence* LevelSequence);

	UPROPERTY(BlueprintAssignable)
	FCinematicLifeCycleEventSignature OnCinematicBegin;
	UPROPERTY(BlueprintAssignable)
	FCinematicLifeCycleEventSignature OnCinematicEnd;

private:
	UFUNCTION()
	void HandleCinematicBegin(const FCinematicContextEventPayload& EventPayload);
	UFUNCTION()
	void HandleCinematicEnd(const FCinematicContextEventPayload& EventPayload);

	UCinematicContext* CreateCinematicContext(ULevelSequencePlayer* LevelSequencePlayer, ULevelSequence* LevelSequence);

	/** A map of sequences and cinematic context objects **/
	UPROPERTY()
	TMap<ULevelSequencePlayer*, UCinematicContext*> CinematicContextMap;
};
