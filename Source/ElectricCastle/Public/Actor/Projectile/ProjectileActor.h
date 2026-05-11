// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/ElectricCastleAbilitySystemTypes.h"
#include "Actor/DamageDealingActor.h"
#include "Actor/Pool/PoolableActor.h"
#include "GameFramework/Actor.h"
#include "ProjectileActor.generated.h"

struct FSpawnPoolEventPayload;
class UCinematicHandlerComponent;
class UCapsuleComponent;
struct FGameplayEffectSpecHandle;
class UProjectileMovementComponent;
class USphereComponent;
class UNiagaraSystem;
class UAudioComponent;

UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API AProjectileActor : public AActor, public IDamageDealingActor, public IPoolableActor
{
	GENERATED_BODY()

public:
	AProjectileActor();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
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
	TArray<AActor*> FindExplosionTargets();

	UFUNCTION(BlueprintCallable)
	void SetupExplosionConfig(
		const TSubclassOf<UGameplayEffect>& InExplosionDamageEffectClass,
		const FElectricCastleDamageConfig& InExplosionDamageConfig
	);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Fire();

	/** Start IDamageDealingActor **/
	virtual void ApplyDamageEffectParams_Implementation(const FDamageEffectParams& InDamageEffectParams) override;
	/** End IDamageDealingActor **/
	/** Start IPoolableActor **/
	virtual UPoolManagerComponent* GetPoolManager_Implementation() const override;
	/** End IPoolableActor **/

protected:
	UFUNCTION(BlueprintNativeEvent)
	void OnSphereOverlap(
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
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Explode();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ExplodeOnTarget(AActor* TargetActor);
	UFUNCTION(BlueprintNativeEvent)
	void OnPool_BeginRetrieve(const FSpawnPoolEventPayload& Payload);
	UFUNCTION(BlueprintNativeEvent)
	void OnPool_FinishRetrieve(const FSpawnPoolEventPayload& Payload);
	UFUNCTION(BlueprintNativeEvent)
	void OnPool_Returned(const FSpawnPoolEventPayload& Payload);
	UFUNCTION(BlueprintCallable)
	void OverrideDamageMagnitude(const float InDamageMagnitudeOverride);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UCapsuleComponent> CollisionComponent;
	UPROPERTY(Category="Components", EditDefaultsOnly)
	TObjectPtr<UAudioComponent> TravelSoundComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UCinematicHandlerComponent> CinematicHandlerComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UPoolManagerComponent> PoolManagerComponent;
	UPROPERTY(EditAnywhere, Category="Properties")
	TObjectPtr<UNiagaraSystem> ImpactEffect;
	UPROPERTY(EditAnywhere, Category="Properties")
	TObjectPtr<USoundBase> ImpactSound;
	UPROPERTY(EditAnywhere, Category="Properties")
	TObjectPtr<USoundBase> TravelSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties")
	bool bExplodeOnImpact = false;
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category="Properties|Explosion",
		meta=(EditCondition="bExplodeOnImpact", EditConditionHides)
	)
	float ImpactRadius = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bExplodeOnImpact", EditConditionHides, ExposeOnSpawn))
	TSubclassOf<UGameplayEffect> ExplosionDamageEffectClass;
	UPROPERTY(
		Replicated,
		BlueprintReadWrite,
		meta=(EditCondition="bExplodeOnImpact", EditConditionHides, ExposeOnSpawn)
	)
	FElectricCastleDamageConfig ExplosionDamageConfig;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties")
	bool bAutoLaunchProjectile = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties")
	bool bDebug = false;

private:
	bool bHit = false;
};
