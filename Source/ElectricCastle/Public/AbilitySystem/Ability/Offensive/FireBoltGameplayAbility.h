// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "ProjectileGameplayAbility.h"
#include "FireBoltGameplayAbility.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API UFireBoltGameplayAbility : public UProjectileGameplayAbility
{
	GENERATED_BODY()

public:
	virtual FString GetDescription_Implementation(const int32 AbilityLevel) const override;
	UFUNCTION(BlueprintCallable)
	void SpawnProjectiles(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, AActor* Target);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "FireBolt")
	float ProjectileSpread = 90.f;
	UPROPERTY(EditDefaultsOnly, Category = "FireBolt")
	float HomingAccelerationMin = 1600.f;
	UPROPERTY(EditDefaultsOnly, Category = "FireBolt")
	float HomingAccelerationMax = 3200.f;
	UPROPERTY(EditDefaultsOnly, Category="FireBolt")
	FScalableFloat ProjectileCount;
	UPROPERTY(EditDefaultsOnly, Category = "FireBolt")
	float TraceRange = 2000.f;

private:
	int32 GetNumProjectiles(const int32 AbilityLevel) const;
	AActor* FindTargetAlongProjectilePath(const FVector& SpawnLocation, const FVector& Forward) const;
};
