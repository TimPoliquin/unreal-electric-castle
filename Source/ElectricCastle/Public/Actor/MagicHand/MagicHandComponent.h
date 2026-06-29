// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Actor/MagicHand/MagicHandTypes.h"
#include "MagicHandComponent.generated.h"

class AMagicHandProjectile;
class UTetherUpdateHandler;
class UNiagaraComponent;
class UNiagaraSystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnMagicHandPossessionChanged,
	AActor*,
	Target,
	bool,
	bPossessed
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMagicHandMissSignature);

// Core component for the Magic Hand system. Attach to the player ACharacter.
//
// Lifecycle:
//   Idle      — default state, no tether active.
//   Casting   — projectile is in flight toward a target.
//   Tethered  — a possessable actor is held; handler ticks every frame.
//   Releasing — (reserved for future fade-out logic; currently transitions straight to Idle).
//
// Requires UMagicHandCharacterMovementComponent on the owning character so that
// the pre-tick movement constraint can be applied via GetAllowedMovementInput.
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UMagicHandComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMagicHandComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Fired when a possession begins (bPossessed=true) or ends (bPossessed=false).
	UPROPERTY(BlueprintAssignable, Category="Magic Hand")
	FOnMagicHandPossessionChanged OnPossessionChanged;
	UPROPERTY(BlueprintAssignable, Category="Magic Hand")
	FOnMagicHandMissSignature OnMissed;

	// Fire the magic hand projectile in the player's look direction.
	UFUNCTION(BlueprintCallable, Category="Magic Hand")
	void ActivateMagicHand();

	// Break the active tether (or cancel a cast in flight).
	UFUNCTION(BlueprintCallable, Category="Magic Hand")
	void ReleaseTether();

	UFUNCTION(BlueprintPure, Category="Magic Hand")
	EMagicHandState GetMagicHandState() const { return CurrentState; }

	UFUNCTION(BlueprintPure, Category="Magic Hand")
	AActor* GetTetheredActor() const { return TargetActor; }

	UFUNCTION(BlueprintPure, Category="Magic Hand")
	FVector GetAnchorLocation() const;

	// Called by AMagicHandProjectile when it overlaps a valid possessable.
	void NotifyProjectileHitPossessable(AActor* PossessableActor, AMagicHandProjectile* HittingProjectile);
	// Called by AMagicHandProjectile when it extends beyond its range and fails
	void NotifyProjectileMissed(AMagicHandProjectile* InMagicHandProjectile);

	// Called by UMagicHandCharacterMovementComponent::ConsumeInputVector each CMC tick.
	// Returns the subset of DesiredInputVector that the active handler permits.
	FVector GetAllowedMovementInput(const FVector& DesiredInputVector) const;

protected:
	// Maximum tether distance in world units.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Magic Hand")
	float MaxTetherLength = 1000.f;

	// Buffer beyond MaxTetherLength before a hard limit is triggered.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Magic Hand")
	float TetherLengthPadding = 50.f;

	// Projectile actor spawned when ActivateMagicHand is called.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Magic Hand")
	TSubclassOf<AMagicHandProjectile> ProjectileClass;

	// Niagara beam system rendered between the player and the tethered target.
	// Expects "BeamStart" and "BeamEnd" FVector user parameters.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Magic Hand|Visuals")
	TObjectPtr<UNiagaraSystem> TetherNiagaraSystem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Magic Hand|Visuals")
	FName BeamStartParamName = FName("Beam Start");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Magic Hand|Visuals")
	FName BeamEndParamName = FName("Beam End");

private:
	EMagicHandState CurrentState = EMagicHandState::Idle;

	UPROPERTY()
	TObjectPtr<AActor> TargetActor;

	UPROPERTY()
	TObjectPtr<UTetherUpdateHandler> ActiveHandler;

	UPROPERTY()
	TObjectPtr<AMagicHandProjectile> ActiveProjectile;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> TetherNiagaraComponent;

	// Cached context from the most recent MagicHandComponent tick. Read by
	// GetAllowedMovementInput during the subsequent CMC ConsumeInputVector call.
	FTetherHandlerContext LastContext;
	bool bContextValid = false;

	FVector PreviousOwnerLocation = FVector::ZeroVector;

	void SetState(EMagicHandState NewState);
	void BeginPossession(AActor* Actor, AMagicHandProjectile* FromProjectile);
	void EndPossession();
	void TickTether();
	void UpdateTetherVisual();
	void DestroyActiveProjectile();
};
