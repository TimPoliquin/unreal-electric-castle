// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/ElectricCastleAbilitySystemTypes.h"
#include "Actor/DamageDealingActor.h"
#include "GameFramework/Actor.h"
#include "ProjectileActor.generated.h"

class UCapsuleComponent;
struct FGameplayEffectSpecHandle;
class UProjectileMovementComponent;
class USphereComponent;
class UNiagaraSystem;
class UAudioComponent;

UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API AProjectileActor : public AActor, public IDamageDealingActor
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
	TArray<AActor*> FindExplosionTargets();

	UFUNCTION(BlueprintCallable)
	void SetupExplosionConfig(
		const TSubclassOf<UGameplayEffect>& InExplosionDamageEffectClass,
		const FElectricCastleDamageConfig& InExplosionDamageConfig
	);

	/** Start IDamageDealingActor **/
	virtual void ApplyDamageEffectParams_Implementation(const FDamageEffectParams& InDamageEffectParams) override;
	/** End IDamageDealingActor **/

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
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Explode();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ExplodeOnTarget(AActor* TargetActor);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UCapsuleComponent> CollisionComponent;
	UPROPERTY(Category="Components", EditDefaultsOnly)
	TObjectPtr<UAudioComponent> TravelSoundComponent;
	UPROPERTY(EditAnywhere, Category="Properties")
	TObjectPtr<UNiagaraSystem> ImpactEffect;
	UPROPERTY(EditAnywhere, Category="Properties")
	TObjectPtr<USoundBase> ImpactSound;
	UPROPERTY(EditAnywhere, Category="Properties")
	TObjectPtr<USoundBase> TravelSound;
	UPROPERTY(EditDefaultsOnly, Category="Properties")
	bool bAutoDestroy = true;
	UPROPERTY(
		EditDefaultsOnly,
		Category="Properties",
		meta=(EditCondition="bAutoDestroy", ClampMin = "0", UIMin = "0.01")
	)
	float LifeSpan = 2.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties")
	bool bExplodeOnImpact = false;
	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category="Properties|Explosion",
		meta=(EditCondition="bExplodeOnImpact", EditConditionHides)
	)
	float ImpactRadius = 1.f;
	UPROPERTY(BlueprintReadWrite, meta=(EditCondition="bExplodeOnImpact", EditConditionHides, ExposeOnSpawn))
	TSubclassOf<UGameplayEffect> ExplosionDamageEffectClass;
	UPROPERTY(
		Replicated,
		BlueprintReadWrite,
		meta=(EditCondition="bExplodeOnImpact", EditConditionHides, ExposeOnSpawn)
	)
	FElectricCastleDamageConfig ExplosionDamageConfig;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties")
	bool bDebug = false;

private:
	bool bHit = false;
};
