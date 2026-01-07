// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "StatusEffectConfig.generated.h"

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FStatusEffectRow
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Categories="Effect"))
	FGameplayTag EffectTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> Icon = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor EffectColor = FLinearColor::White;
};

/**
 * 
 */
UCLASS(Abstract, BlueprintType)
class ELECTRICCASTLE_API UStatusEffectConfig : public UDataAsset
{
	GENERATED_BODY()
public:
	bool FindStatusEffectByTag(const FGameplayTag& EffectTag, FStatusEffectRow& OutStatusEffect) const;
	FGameplayTagContainer GetAllStatusEffectTags() const;
	FGameplayTagContainer GetMatchingStatusEffectTags(const FGameplayTagContainer& Tags) const;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FStatusEffectRow> StatusEffects;
};
