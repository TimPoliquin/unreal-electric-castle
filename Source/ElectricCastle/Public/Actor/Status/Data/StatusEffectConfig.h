// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "StatusEffectConfig.generated.h"

class UStatusEffectPlugin;
struct FStatusEffectDuration;

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FStatusEffectRow
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Categories="Effect"))
	FGameplayTag EffectTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced)
	TArray<TObjectPtr<UStatusEffectPlugin>> Plugins;

	bool IsValid() const
	{
		return EffectTag.IsValid();
	}

	void ApplyEffect(AActor* InActor, const FStatusEffectDuration& Duration) const;
	void RemoveEffect(AActor* InActor) const;
};

/**
 * 
 */
UCLASS(BlueprintType)
class ELECTRICCASTLE_API UStatusEffectConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	FStatusEffectRow FindStatusEffectRowByTags(const FGameplayTagContainer& EffectTags) const;

	FGameplayTagContainer GetAllStatusEffectTags() const;
	FGameplayTagContainer GetMatchingStatusEffectTags(const FGameplayTagContainer& Tags) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FStatusEffectRow> StatusEffects;
};
