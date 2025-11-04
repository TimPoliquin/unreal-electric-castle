// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelUpInfo.generated.h"

USTRUCT(BlueprintType)
struct FLevelUpRewards
{
	GENERATED_BODY()

	int32 AttributePoints;
	int32 SpellPoints;

	FLevelUpRewards()
	{
		AttributePoints = 0;
		SpellPoints = 0;
	}

	FLevelUpRewards(const int32 InAttributePoints, const int32 InSpellPoints)
	{
		AttributePoints = InAttributePoints;
		SpellPoints = InSpellPoints;
	}
};

USTRUCT(BlueprintType)
struct FLevelUpInfoRow
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	int32 LevelUpRequirement = 0;
	UPROPERTY(EditDefaultsOnly)
	int32 AttributePointRewards = 1;
	UPROPERTY(EditDefaultsOnly)
	int32 SpellPointRewards = 1;

	FLevelUpRewards GetRewards() const
	{
		return FLevelUpRewards(AttributePointRewards, SpellPointRewards);
	}
};


/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API ULevelUpInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FLevelUpInfoRow> LevelUpInfos;

	FLevelUpInfoRow FindLevelUpInfoByXP(const int32 XP) const;
	int32 FindLevelByXP(const int32 XP) const;
	FLevelUpRewards GetRewardsByLevel(const int32 InLevel) const;

	float GetLevelProgressPercentage(const int32 CurrentXP) const;
};
