// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrackActor.generated.h"

class USplineComponent;

UCLASS()
class ELECTRICCASTLE_API ATrackActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATrackActor();
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

protected:
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable)
	void SetTrackOnTrackFollowingActor() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USplineComponent> Track;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta=(MustImplement="/Script/ElectricCastle.TrackFollowingActor"))
	TObjectPtr<AActor> TrackFollowingActor;
};
