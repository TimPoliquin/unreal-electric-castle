// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PawnFollowSplineComponent.generated.h"


class UMovementComponent;
class USplineComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API UPawnFollowSplineComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPawnFollowSplineComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION(BlueprintCallable)
	void FollowSpline(AActor* SplineActor);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
	TWeakObjectPtr<USplineComponent> SplineComponent;

private:
	void MoveAlongSpline(float DeltaTime);

	UPROPERTY(VisibleAnywhere)
	float DistanceAlongSpline = 0.f;
	UPROPERTY()
	TObjectPtr<UPawnMovementComponent> MovementComponent;
};
