// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "FormConfigTypes.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/DataAsset.h"
#include "PlayerFormConfig.generated.h"

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FPlayerFormConfigRow
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EPlayerForm FormId;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(Categories="Player.Form"))
	FGameplayTag FormTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<USkeletalMesh> CharacterMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftClassPtr<UAnimInstance> AnimationBlueprint;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> PortraitImage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TObjectPtr<UGameplayAbility>> Abilities;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TObjectPtr<UGameplayAbility>> PassiveAbilities;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> FormAttributes;

	FPlayerFormConfigRow()
	{
		FormId = EPlayerForm::None;
	}

	explicit FPlayerFormConfigRow(const EPlayerForm InForm)
	{
		FormId = InForm;
	}

	bool IsLoaded() const
	{
		return
			(CharacterMesh.IsNull() || CharacterMesh.IsValid()) &&
			(AnimationBlueprint.IsNull() || AnimationBlueprint.IsValid()) &&
			(PortraitImage.IsNull() || PortraitImage.IsValid());
	}

	bool IsValid() const
	{
		return
			FormTag.IsValid() &&
			FormId != EPlayerForm::None && FormId != EPlayerForm::Invalid_Max;
	}

	bool operator==(const FPlayerFormConfigRow& Other) const
	{
		return FormId == Other.FormId;
	}

	friend FORCEINLINE uint32 GetTypeHash(const FPlayerFormConfigRow& Key)
	{
		return GetTypeHash(Key.FormId);
	}
};

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UPlayerFormConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPlayerFormConfig();

	virtual void Initialize();

	UFUNCTION(BlueprintCallable)
	FPlayerFormConfigRow GetPlayerFormConfigRowByTag(const FGameplayTag& FormTag) const;
	UFUNCTION(BlueprintCallable)
	FPlayerFormConfigRow GetPlayerFormConfigRowByFormId(const int32 FormId) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FPlayerFormConfigRow Barbarian = FPlayerFormConfigRow(EPlayerForm::Barbarian);
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FPlayerFormConfigRow Egyptian = FPlayerFormConfigRow(EPlayerForm::Egyptian);
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FPlayerFormConfigRow Futureman = FPlayerFormConfigRow(EPlayerForm::Futureman);
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FPlayerFormConfigRow Highlander = FPlayerFormConfigRow(EPlayerForm::Highlander);
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FPlayerFormConfigRow Hippie = FPlayerFormConfigRow(EPlayerForm::Hippie);
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FPlayerFormConfigRow Knight = FPlayerFormConfigRow(EPlayerForm::Knight);
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FPlayerFormConfigRow Native = FPlayerFormConfigRow(EPlayerForm::Native);
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FPlayerFormConfigRow Roman = FPlayerFormConfigRow(EPlayerForm::Roman);

private:
	UPROPERTY()
	TMap<FGameplayTag, FPlayerFormConfigRow> PlayerFormConfigByTag;
	UPROPERTY()
	TMap<EPlayerForm, FPlayerFormConfigRow> PlayerFormConfigByEnum;
};
