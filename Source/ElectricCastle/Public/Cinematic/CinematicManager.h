// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "CinematicEvents.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CinematicManager.generated.h"

class UCinematicSequenceMetadata;
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

	UFUNCTION(BlueprintCallable)
	void RegisterContext(UCinematicContext* InContext);

	UPROPERTY(BlueprintAssignable)
	FCinematicLifeCycleEventSignature OnCinematicBegin;
	UPROPERTY(BlueprintAssignable)
	FCinematicLifeCycleEventSignature OnCinematicEnd;

private:
	UFUNCTION()
	void HandleCinematicBegin(const FCinematicContextEventPayload& EventPayload);
	UFUNCTION()
	void HandleCinematicEnd(const FCinematicContextEventPayload& EventPayload);
};
