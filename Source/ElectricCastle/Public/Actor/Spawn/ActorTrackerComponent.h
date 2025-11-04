// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpawnEventPayloadDefinitions.h"
#include "ActorTrackerComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API UActorTrackerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UActorTrackerComponent();

	UFUNCTION(BlueprintCallable)
	void Track(AActor* Actor);
	UPROPERTY(BlueprintAssignable)
	FOnAuraActorTrackerCountChangedDelegate OnCountChanged;

protected:
	UPROPERTY(VisibleInstanceOnly)
	TArray<AActor*> Actors;

private:
	UFUNCTION()
	void OnTrackedActorDestroyed(AActor* DestroyedActor);
};
