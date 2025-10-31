// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Utils/RichTextMacros.h"
#include "ElectricCastleGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UElectricCastleGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	FORCEINLINE TArray<FGameplayTag> GetStartupInputTag() const
	{
		return StartupInputTags;
	}

	virtual FString GetAbilityName() const
	{
		return AbilityName.ToString();
	}

	UFUNCTION(BlueprintNativeEvent)
	FString GetDescription(const int32 AbilityLevel) const;

	static FString GetLockedDescription(const int32 LevelRequirement)
	{
		return FString::Printf(
			TEXT(
				RICH_DEFAULT("Spell Locked Until Level") RICH_LEVEL("%d")
			),
			LevelRequirement
		);
	}

	float GetManaCost(const float InLevel = 1.f) const;
	float GetCooldown(const float InLevel = 1.f) const;

protected:
	void ExecuteTask(UAbilityTask* Task) const;
	/**
	 * Determines whether the ability should use motion warping. This is primarily true if the user is using keyboard & mouse input, and has a target selected.
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName="Should Set Motion Warping Target?")
	bool ShouldSetMotionTarget() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void FaceHitTarget(const FHitResult& HitResult);

private:
	UPROPERTY(EditDefaultsOnly, Category="Properties")
	FName AbilityName;
	UPROPERTY(EditDefaultsOnly, Category="Properties", meta=(Categories="InputTag"))
	TArray<FGameplayTag> StartupInputTags;
};
