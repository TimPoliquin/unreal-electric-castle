// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystem/ElectricCastleGameplayAbilityTypes.h"
#include "Utils/RichTextMacros.h"
#include "ElectricCastleGameplayAbility.generated.h"

enum class EEngagementAbilityMode : uint8;
enum class EEngagementRange : uint8;
class UAbilityRangeConfig;
class UAbilityEngagementConfig;


/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UElectricCastleGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UElectricCastleGameplayAbility();

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
	UFUNCTION(BlueprintCallable)
	void ApplyCustomCooldown() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FFloatRange GetPreferredDistanceRange() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsSupportedEngagementRange(const EEngagementRange InEngagementRange) const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsSupportedEngagementMode(const TArray<EEngagementAbilityMode>& InEngagementAbilityModes) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties")
	FAbilityCooldownConfig CooldownConfig;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category="Properties")
	TObjectPtr<UAbilityRangeConfig> AbilityRangeConfig;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category="Properties")
	TObjectPtr<UAbilityEngagementConfig> AbilityEngagementConfig;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties")
	bool bAutoApplyCooldownOnAbilityEnd = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties")
	bool bDebug = false;
	virtual void CommitExecute(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo
	) override;
	void ExecuteTask(UAbilityTask* Task) const;
	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	) override;
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

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FRotator CalculateRotationToTarget(const AActor* Target, FVector StartingLocation, float ProjectileSpeed = 0, bool bAffectedByGravity = false) const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetAvatarActorForwardVector() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FRotator GetAvatarActorForwardRotator() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetAvatarActorSocketLocation(const FGameplayTag& SocketTag, bool bIsWeaponSocket = false) const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FRotator GetAvatarActorSocketRotation(const FGameplayTag& SocketTag, bool bIsWeaponSocket = false) const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FRotator CalculateSpawnRotationFacingAimTarget(const FVector SpawnLocation) const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FTransform CalculateSpawnTransform(const FVector SpawnLocation) const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void RotateTowardAvatarActorAimTarget(AActor* ActorToRotate) const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	AActor* GetAimTarget() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void CameraKick(const FVector& Direction, const FScalableFloat& Magnitude, const float Duration);

private:
	UPROPERTY(EditDefaultsOnly, Category="Properties")
	FName AbilityName;
};
