// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "DamageGameplayAbility.h"
#include "FireBlastGameplayAbility.generated.h"

class AFireballProjectile;
/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API UFireBlastGameplayAbility : public UDamageGameplayAbility
{
	GENERATED_BODY()

public:
	virtual FString GetDescription_Implementation(const int32 AbilityLevel) const override;
	UFUNCTION(BlueprintCallable, Category = "FireBlast")
	TArray<AFireballProjectile*> SpawnFireballs();

protected:
	UPROPERTY(EditDefaultsOnly, Category="FireBlast")
	int32 NumFireballs = 12;

private:
	UPROPERTY(EditDefaultsOnly, Category = "FireBlast")
	TSubclassOf<AFireballProjectile> FireballClass;
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FElectricCastleDamageConfig ExplosionDamageConfig;
};
