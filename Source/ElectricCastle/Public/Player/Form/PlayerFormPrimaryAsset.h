// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "FormConfigTypes.h"
#include "GameplayTagContainer.h"
#include "InputMappingContext.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/DataAsset.h"
#include "PlayerFormPrimaryAsset.generated.h"

class UAbilityInfo;
class UInputConfiguration;
class UPlayerFormChangeComponent;
class UFormConfig;
/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UPlayerFormPrimaryAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	UFUNCTION(BlueprintNativeEvent)
	void OnFormActivated(AActor* Actor, UPlayerFormChangeComponent* FormChangeComponent) const;
	UFUNCTION(BlueprintNativeEvent)
	void OnFormDeactivated(AActor* Actor, UPlayerFormChangeComponent* FormChangeComponent) const;
	UFUNCTION(BlueprintCallable)
	UInputMappingContext* GetInputMappingContext() const;
	UFUNCTION(BlueprintCallable)
	UInputConfiguration* GetInputConfiguration() const;
	UFUNCTION(BlueprintCallable)
	UAbilityInfo* GetAbilityInfo() const;

	bool IsValid() const;
	void Initialize();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EPlayerForm FormId = EPlayerForm::None;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString FormName = FString("");
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Categories="Player.Form"))
	FGameplayTag FormTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Categories="Abilities.Form"))
	FGameplayTag FormAbilityTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AssetBundles="FormsBundle"))
	FFormMeshConfig MeshConfig;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AssetBundles="FormsBundle"))
	TSoftClassPtr<UAnimInstance> AnimationBlueprint;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> PortraitImage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Categories="Item.Type.Equipment"))
	FGameplayTag WeaponTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TSoftObjectPtr<UInputMappingContext> InputMappingContext;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TSoftObjectPtr<UInputConfiguration> InputConfiguration;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UAbilityInfo> Abilities;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> FormAttributes;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TObjectPtr<UFormConfig>> FormConfigs;
};
