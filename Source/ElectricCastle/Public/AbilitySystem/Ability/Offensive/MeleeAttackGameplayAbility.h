// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "DamageGameplayAbility.h"
#include "MeleeAttackGameplayAbility.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API UMeleeAttackGameplayAbility : public UDamageGameplayAbility
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void DamageTargets(const TArray<AActor*>& Targets, const FVector& ImpactLocation, const FGameplayTag& MontageTag);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Categories="GameplayCue"))
	FGameplayTag ImpactCueTag = FGameplayTag::EmptyTag;
};
