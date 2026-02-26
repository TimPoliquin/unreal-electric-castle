// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Item/ItemTypes.h"
#include "Player/Form/FormConfigTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ElectricCastleGameDataSubsystem.generated.h"

class UStatusEffectConfig;
class UPlayerFormPrimaryAsset;
class UDebuffConfig;
class UPlayerFormConfig;
class ULevelUpInfo;
class UTreasureConfig;
class UItemDefinitions;
class UGameplayEffect;
class UAbilityInfo;
class UCharacterClassInfo;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameDataLoaded);

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API UElectricCastleGameDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	static UElectricCastleGameDataSubsystem* Get(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UPlayerFormPrimaryAsset* GetPlayerFormConfigById(const EPlayerForm& FormId) const;
	UPlayerFormPrimaryAsset* GetPlayerFormConfigById(const int32 FormId) const;
	UPlayerFormPrimaryAsset* GetPlayerFormConfigByTag(const FGameplayTag& FormTag) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TSubclassOf<UGameplayEffect> GetFormChangeHealthEffectClass() const { return HealthChangeEffect; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TSubclassOf<UGameplayEffect> GetFormChangeManaEffectClass() const { return ManaChangeEffect; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UDebuffConfig* GetDebuffConfig() const { return DebuffConfig; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UStatusEffectConfig* GetStatusEffectConfig() const { return StatusEffectConfig; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UAbilityInfo* GetAbilityInfo() const { return AbilityInfo; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UCharacterClassInfo* GetCharacterClassInfo() const { return CharacterClassInfo; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TSubclassOf<UGameplayEffect> GetDefaultInteractEffectClass() const { return DefaultInteractEffectClass; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UMaterialParameterCollection* GetPsychedelicVisionParameters() const { return PsychedelicVisionParameters; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetDefaultPlayerLevel() const { return DefaultPlayerLevel; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UTreasureConfig* GetTreasureConfig() const { return TreasureConfig; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FItemDefinition FindItemDefinitionByItemTag(const FGameplayTag& ItemTag);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FGameplayTag GetDefaultItemPickupMessageTag() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FGameplayTag GetDefaultItemUseMessageTag() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsGameDataLoaded() const { return bIsGameDataLoaded; }

	UPROPERTY(BlueprintAssignable)
	FOnGameDataLoaded OnGameDataLoaded;

	float GetXPToNextLevelPercentage(float XP) const;
	int32 FindLevelByXP(const int32 InXP) const;
	FLevelUpRewards GetLevelUpRewards(const int32 InLevel) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Form Configuration")
	TArray<TSoftObjectPtr<UPlayerFormPrimaryAsset>> PlayerFormPrimaryAssets;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Form Configuration")
	TSubclassOf<UGameplayEffect> HealthChangeEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Form Configuration")
	TSubclassOf<UGameplayEffect> ManaChangeEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Form Configuration")
	bool bPreloadForms = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Gameplay Effects")
	TObjectPtr<UDebuffConfig> DebuffConfig;
	UPROPERTY(EditDefaultsOnly, Category="Gameplay Effects")
	TObjectPtr<UStatusEffectConfig> StatusEffectConfig;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character Class Defaults")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 DefaultPlayerLevel = 1;
	UPROPERTY(EditDefaultsOnly, Category="Character Class Defaults")
	TObjectPtr<ULevelUpInfo> LevelUpInfo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ability Info")
	TObjectPtr<UAbilityInfo> AbilityInfo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ability Info")
	TSubclassOf<UGameplayEffect> DefaultInteractEffectClass;
	UPROPERTY(EditDefaultsOnly, Category="Ability Info")
	TObjectPtr<UMaterialParameterCollection> PsychedelicVisionParameters;
	UPROPERTY(EditDefaultsOnly, Category="Items")
	TArray<UItemDefinitions*> ItemInfos;
	UPROPERTY(EditDefaultsOnly, Category="Items|Treasure")
	TObjectPtr<UTreasureConfig> TreasureConfig;
	UPROPERTY(EditDefaultsOnly, Category="Items|Messages", meta=(Categories="Message"))
	FGameplayTag DefaultItemPickupMessageTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditDefaultsOnly, Category="Items|Messages", meta=(Categories="Message"))
	FGameplayTag DefaultItemUsedMessageTag = FGameplayTag::EmptyTag;

private:
	void InitializeItemDefinitions();
	void OnFormDataInitialized();
	bool ShouldPreloadForms(TArray<FPrimaryAssetId>& OutAssetIds) const;
	UPROPERTY()
	TMap<FGameplayTag, FItemDefinition> ItemDefinitions;
	bool bIsGameDataLoaded = false;
};
