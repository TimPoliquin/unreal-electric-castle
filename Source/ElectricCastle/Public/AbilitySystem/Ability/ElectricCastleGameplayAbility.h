// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Utils/RichTextMacros.h"
#include "ElectricCastleGameplayAbility.generated.h"

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FComboAbilityConfig
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName ComboSectionName = NAME_None;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(Categories="Event.Montage"))
	FGameplayTag MontageEventTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bIsWeaponAbility = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName SocketName = NAME_None;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bOverrideAbilityImpactRadius = false;
	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadWrite,
		meta=(EditCondition="bOverrideAbilityImpactRadius", EditConditionHides)
	)
	float ImpactRadius = 25.f;
};

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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties")
	bool bDebug = false;

	void ExecuteTask(UAbilityTask* Task) const;
	/**
	 * Determines whether the ability should use motion warping. This is primarily true if the user is using keyboard & mouse input, and has a target selected.
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName="Should Set Motion Warping Target?")
	bool ShouldSetMotionTarget() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void FaceHitTarget(const FHitResult& HitResult);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static void GetActorDifference(
		const TArray<AActor*>& AList,
		const TSet<AActor*>& BList,
		TArray<AActor*>& OutDifference
	);

	UFUNCTION(BlueprintCallable)
	void DebugLog(const FString LogString, const float TimeToDisplay = 1.f, const FColor Color = FColor::Red) const;

	UFUNCTION(BlueprintCallable)
	bool GetComboMontageHitLocationFromList(
		int32 ComboIdx,
		TArray<FComboAbilityConfig> ComboConfigs,
		FVector& ComboHitLocation
	) const;
	UFUNCTION(BlueprintCallable)
	bool GetComboMontageHitLocation(
		const FComboAbilityConfig& ComboConfigs,
		FVector& ComboHitLocation
	) const;

private:
	UPROPERTY(EditDefaultsOnly, Category="Properties")
	FName AbilityName;
	UPROPERTY(EditDefaultsOnly, Category="Properties", meta=(Categories="InputTag"))
	TArray<FGameplayTag> StartupInputTags;
};
