// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/DataAsset.h"
#include "AbilityInfo.generated.h"

USTRUCT(BlueprintType)
struct FElectricCastleAbilityInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Categories="Abilities"))
	FGameplayTag AbilityTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, meta=(Categories="Abilities"))
	FGameplayTag StatusTag = FGameplayTag();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Categories="Cooldown"))
	FGameplayTag CooldownTag = FGameplayTag();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Categories="Abilities.Type"))
	FGameplayTag AbilityType = FGameplayTag();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> Icon = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UMaterialInterface> BackgroundMaterial = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Categories="InputTag"))
	FGameplayTag InputTag = FGameplayTag();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 LevelRequirement = 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayAbility> Ability;

	bool IsValid() const
	{
		return AbilityTag.IsValid();
	}
};

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UAbilityInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ability Information")
	TArray<FElectricCastleAbilityInfo> AbilityInformation;

	FElectricCastleAbilityInfo FindAbilityInfoForTag(const FGameplayTag AbilityTag, bool bLogNotFound = false) const;
};
