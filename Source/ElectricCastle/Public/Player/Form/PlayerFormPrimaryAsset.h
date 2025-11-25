// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "FormConfigTypes.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/DataAsset.h"
#include "PlayerFormPrimaryAsset.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UPlayerFormPrimaryAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FFormMeshConfig MeshConfig;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AssetBundles="FormsBundle"))
	TSoftClassPtr<UAnimInstance> AnimationBlueprint;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> PortraitImage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Categories="Item.Type.Equipment"))
	FGameplayTag WeaponTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Categories="Abilities"))
	TArray<FGameplayTag> Abilities;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UGameplayAbility>> PassiveAbilities;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> FormAttributes;
};
