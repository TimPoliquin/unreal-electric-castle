#pragma once


#include "FormConfigTypes.generated.h"

class UGroomComponent;
class UGroomBindingAsset;
class UGroomAsset;

UENUM(BlueprintType)
enum class EPlayerForm : uint8
{
	None = 0,
	Barbarian = 1,
	Egyptian = 2,
	Futureman = 3,
	Highlander = 4,
	Hippie = 5,
	Knight = 6,
	Native = 7,
	Roman = 8,
	Count = 9 UMETA(Hidden)
};

ENUM_RANGE_BY_FIRST_AND_LAST(EPlayerForm, EPlayerForm::Barbarian, EPlayerForm::Roman)

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FFormMaterialConfig
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AssetBundles = "FormsBundle"))
	TSoftObjectPtr<UMaterialInterface> MaterialAsset;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName MaterialSlotName = NAME_None;
	bool IsNull() const;
	bool IsValid() const;
	bool IsLoaded() const;
	void SetToComponent(UMeshComponent* MeshComponent, const int32 Idx) const;
};

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FFormMeshPartConfig
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AssetBundles = "FormsBundle"))
	TSoftObjectPtr<USkeletalMesh> MeshAsset;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FFormMaterialConfig> Materials;
	bool IsNull() const;
	bool IsValid() const;
	bool IsLoaded() const;
	void SetToComponent(USkeletalMeshComponent* MeshComponent) const;
	void Initialize() const;
};

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FFormGroomConfig
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AssetBundles = "FormsBundle"))
	TSoftObjectPtr<UGroomAsset> GroomAsset;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AssetBundles = "FormsBundle"))
	TSoftObjectPtr<UGroomBindingAsset> GroomBindingAsset;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FFormMaterialConfig> Materials;

	bool IsNull() const;
	bool IsValid() const;
	bool IsLoaded() const;
	void Initialize() const;
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
	void Initialize() const;
};
