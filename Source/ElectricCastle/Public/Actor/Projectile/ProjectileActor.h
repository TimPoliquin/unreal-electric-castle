// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/ElectricCastleAbilitySystemTypes.h"
#include "GameFramework/Actor.h"
#include "ProjectileActor.generated.h"

struct FGameplayEffectSpecHandle;
class UProjectileMovementComponent;
class USphereComponent;
class UNiagaraSystem;
class UAudioComponent;

UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API AProjectileActor : public AActor
{
	GENERATED_BODY()

public:
	AProjectileActor();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true), Replicated)
	FDamageEffectParams DamageEffectParams;

	UProjectileMovementComponent* GetProjectileMovementComponent() const;
	UPROPERTY()
	TObjectPtr<USceneComponent> HomingTargetSceneComponent;
	UPROPERTY(EditDefaultsOnly)
	bool bShouldDestroyOnTargetDeath = true;
	UFUNCTION(BlueprintCallable)
	void SetHomingTarget(AActor* Target);
	UFUNCTION()
	void OnTargetDead(AActor* DeadActor);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Projectile")
	FVector GetImpactDirection(const AActor* HitActor) const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	TArray<AActor*> FindImpactTargets();

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	bool IsValidOverlap(const AActor* OtherActor) const;
	UFUNCTION(BlueprintCallable)
	void PlayImpactEffect();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ImpactRadius = 1.f;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> ImpactEffect;
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> ImpactSound;
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> TravelSound;
	UPROPERTY()
	TObjectPtr<UAudioComponent> TravelSoundComponent;
	bool bHit = false;
	UPROPERTY(EditDefaultsOnly)
	float LifeSpan = 2.f;
};
