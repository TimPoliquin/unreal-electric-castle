// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MagicHandProjectile.generated.h"

class UMagicHandComponent;
class USphereComponent;
class UProjectileMovementComponent;
class UNiagaraComponent;
class UNiagaraSystem;

// The visual projectile that travels when the player activates the Magic Hand.
//
// Lifecycle:
//   1. Spawned by UMagicHandComponent and launched via Launch().
//   2. Flies straight (no gravity) until it either overlaps a possessable actor
//      or exceeds MaxTravelDistance and self-destructs.
//   3. On a successful possession, EnterTetherAnchorMode() is called. The projectile
//      stops moving, attaches to the target, and swaps to an anchor visual effect.
//      It is then destroyed when the tether is released via UMagicHandComponent.
UCLASS()
class AMagicHandProjectile : public AActor
{
	GENERATED_BODY()

public:
	AMagicHandProjectile();

	// Initial and maximum projectile travel speed (cm/s).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile")
	float ProjectileSpeed = 3000.f;

	// The projectile destroys itself once it has traveled this far without a hit.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile")
	float MaxTravelDistance = 3000.f;

	// Niagara system played while the projectile is in flight.
	UPROPERTY(EditDefaultsOnly, Category="Visuals")
	TObjectPtr<UNiagaraSystem> TravelEffectSystem;

	// Niagara system swapped in once the projectile anchors to the possessed target.
	UPROPERTY(EditDefaultsOnly, Category="Visuals")
	TObjectPtr<UNiagaraSystem> AnchorEffectSystem;

	// Called by UMagicHandComponent after spawning. Activates movement and travel VFX.
	void Launch(const FVector& Direction, UMagicHandComponent* InOwnerComponent);

	// Called when possession is confirmed. Freezes the projectile, attaches it to
	// TetherTarget, and activates the anchor VFX.
	void EnterTetherAnchorMode(AActor* TetherTarget);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USphereComponent> CollisionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

private:
	UPROPERTY()
	TWeakObjectPtr<UMagicHandComponent> OwnerMagicHandComponent;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> ActiveEffectComponent;

	FVector LaunchLocation = FVector::ZeroVector;
	bool bInAnchorMode = false;

	UFUNCTION()
	void OnSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult
	);
};
