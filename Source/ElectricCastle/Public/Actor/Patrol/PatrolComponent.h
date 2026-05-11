// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/TargetPoint.h"
#include "Utils/RandUtils.h"
#include "PatrolComponent.generated.h"

class ATargetPoint;

UENUM(BlueprintType)
enum class EPatrolType : uint8
{
	None,
	Random,
	Predefined,
	Count
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API UPatrolComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPatrolComponent();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool CanPatrol() const;
	UFUNCTION(BlueprintCallable)
	FVector GetNextPatrolLocation();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetPatrolMovementSpeed() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetPatrolWaitTime() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EPatrolType PatrolType = EPatrolType::None;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FFloatRange PatrolSpeed = FFloatRange::Inclusive(200.f, 300.f);
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FFloatRange PatrolWaitTime = FFloatRange::Inclusive(2.f, 5.f);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="PatrolType == EPatrolType::Predefined", EditConditionHides))
	TArray<ATargetPoint*> PatrolTargets;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="PatrolType == EPatrolType::Predefined", EditConditionHides))
	bool bPatrolTargetsInOrder = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="PatrolType == EPatrolType::Random", EditConditionHides))
	float PatrolRadius = 1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bDebug = false;

private:
	FVector GetRandomPatrolLocation() const;
	FVector GetPatrolLocation(const uint32 InPatrolTargetIndex) const;
	UPROPERTY(VisibleInstanceOnly)
	uint32 PatrolTargetIndex = -1;
};
