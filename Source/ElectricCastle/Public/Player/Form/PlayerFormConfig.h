// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "FormConfigTypes.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/DataAsset.h"
#include "PlayerFormConfig.generated.h"

class AEquipmentActor;
class UGroomComponent;
class UGroomBindingAsset;
class UGroomAsset;
class UFormConfigLoadRequest;

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FFormMaterialConfig
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UMaterialInterface> MaterialAsset;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName MaterialSlotName = NAME_None;
	bool IsNull() const;
	bool IsValid() const;
	bool IsLoaded() const;
	void AddToLoad(UFormConfigLoadRequest* LoadRequest) const;
	void SetToComponent(UMeshComponent* MeshComponent, const int32 Idx) const;
};

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FFormMeshPartConfig
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<USkeletalMesh> MeshAsset;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FFormMaterialConfig> Materials;
	bool IsNull() const;
	bool IsValid() const;
	bool IsLoaded() const;
	void AddToLoad(UFormConfigLoadRequest* LoadRequest) const;
	void SetToComponent(USkeletalMeshComponent* MeshComponent) const;
};

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FFormGroomConfig
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UGroomAsset> GroomAsset;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UGroomBindingAsset> GroomBindingAsset;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FFormMaterialConfig> Materials;

	bool IsNull() const;
	bool IsValid() const;
	bool IsLoaded() const;
	void AddToLoad(UFormConfigLoadRequest* LoadRequest) const;
	void SetToComponent(UGroomComponent* GroomComponent) const;
};

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FFormMeshConfig
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FFormMeshPartConfig Body;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FFormMeshPartConfig Face;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FFormMeshPartConfig Clothing;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Groom")
	FFormGroomConfig Beard;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Groom")
	FFormGroomConfig Fuzz;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Groom")
	FFormGroomConfig Eyebrows;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Groom")
	FFormGroomConfig Eyelashes;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Groom")
	FFormGroomConfig Hair;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Groom")
	FFormGroomConfig Moustache;

	bool IsLoaded() const;
	void AddToLoad(UFormConfigLoadRequest* LoadRequest) const;
};

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FPlayerFormConfigRow
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EPlayerForm FormId;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(Categories="Player.Form"))
	FGameplayTag FormTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Categories="Abilities.Form"))
	FGameplayTag FormAbilityTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FFormMeshConfig MeshConfig;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftClassPtr<UAnimInstance> AnimationBlueprint;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> PortraitImage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag WeaponTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName WeaponHandSocketName = FName("Socket_Hand_Right");
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Categories="Abilities"))
	TArray<FGameplayTag> Abilities;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UGameplayAbility>> PassiveAbilities;
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

	bool IsLoaded() const;
	bool IsValid() const;

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
	UFormConfigLoadRequest* GetOrCreateLoadRequest(const FGameplayTag& FormTag);
	FPlayerFormConfigRow GetPlayerFormConfigRowByFormId(const int32 FormId) const;
	void LoadAsync(UFormConfigLoadRequest* LoadRequest);

	UFUNCTION(BlueprintCallable)
	TSubclassOf<UGameplayEffect> GetHealthChangeEffect() const { return HealthChangeEffect; }

	UFUNCTION(BlueprintCallable)
	TSubclassOf<UGameplayEffect> GetManaChangeEffect() const { return ManaChangeEffect; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Global Form Change Effects")
	TSubclassOf<UGameplayEffect> HealthChangeEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Global Form Change Effects")
	TSubclassOf<UGameplayEffect> ManaChangeEffect;
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
	UPROPERTY()
	TMap<FGameplayTag, UFormConfigLoadRequest*> FormLoadRequests;
	UFUNCTION()
	void OnLoadComplete(const FPlayerFormConfigRow& Row);
};
