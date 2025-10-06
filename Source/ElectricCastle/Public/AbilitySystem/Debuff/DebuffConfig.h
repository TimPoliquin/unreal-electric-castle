// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "DebuffConfig.generated.h"

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FDebuffConfigRow
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Categories="Effect.Debuff.Type"))
	FGameplayTag DebuffTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DebuffEffect;

	bool IsValid() const
	{
		return DebuffTag.IsValid() && DebuffEffect;
	}
};

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UDebuffConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	FDebuffConfigRow GetDebuffConfigByDebuffTag(const FGameplayTag& DebuffTag) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FDebuffConfigRow> DebuffConfigRows;
};
