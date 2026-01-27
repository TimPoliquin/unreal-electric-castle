// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BasicGravityComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGravityComponentLanded);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API UBasicGravityComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBasicGravityComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void Activate(bool bReset = false) override;
	virtual void Deactivate() override;

	UPROPERTY(BlueprintAssignable)
	FGravityComponentLanded OnLandedDelegate;

protected:
	// Gravity strength (cm/s^2). Default matches UE's gravity.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gravity")
	float GravityAcceleration = -980.f;

	// Max downward speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gravity")
	float TerminalVelocity = -4000.f;

	// Distance to check for ground
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gravity")
	float GroundCheckDistance = 50.f;

	// Whether the actor is currently grounded
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Gravity")
	bool bIsGrounded = false;

	// Current vertical velocity
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Gravity")
	float VerticalVelocity = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gravity")
	bool bAutoDeactivate = true;

private:
	bool PerformGroundCheck(FHitResult& OutHit) const;
	void ApplyGravity(float DeltaTime);
	void ResolveGroundCollision(const FHitResult& Hit);
};
