// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "EngagementMovementPlugin.h"
#include "OrbitEngagementMovementPlugin.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UOrbitEngagementMovementPlugin : public UEngagementMovementPlugin
{
	GENERATED_BODY()

public:
	virtual void TickMovement_Implementation(const float DeltaTime) override;
	virtual void ChangeItUp_Implementation() override;
	virtual void SetTargetActor_Implementation(AActor* InTargetActor) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FFloatRange OrbitSpeedRange = FFloatRange::Inclusive(50.f, 75.f);
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float OrbitSpeed = 75.f;
	UPROPERTY(EditAnywhere)
	float TangentSmoothingSpeed = 4.f; // tweakable
	UPROPERTY()
	FVector SmoothedTangentDirection = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RadialDeadZone = 25.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AnticipationDistance = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseNavSystem = true;

private:
	FVector CalculateSmoothOrbitVelocity(const FVector& RadialDirection, const float DeltaTime);
	FVector CalculateDesiredVelocity(const float DeltaTime);
	bool IsDesiredLocationNavigable(const FVector& DesiredLocation) const;
	UPROPERTY(VisibleInstanceOnly)
	bool bOrbitClockwise = true;
};
