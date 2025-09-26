#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "ElectricCastleGameplayEffectTypes.generated.h"

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerOverlapSignature, AActor*, Player);

UENUM(BlueprintType)
enum class EEffectApplicationPolicy : uint8
{
	ApplyOnOverlap,
	ApplyOnEndOverlap,
	DoNotApply
};

UENUM(BlueprintType)
enum class EEffectRemovalPolicy : uint8
{
	RemoveOnEndOverlap,
	DoNotRemove
};

USTRUCT(BlueprintType)
struct FGameplayEffectConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> GameplayEffectClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	EEffectApplicationPolicy EffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;
	/** RemovalPolicy is only applicable to Infinite effects **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	EEffectRemovalPolicy RemovalPolicy = EEffectRemovalPolicy::DoNotRemove;
	bool IsValid() const { return GameplayEffectClass != nullptr; }
	bool IsApplyOnOverlap() const { return EffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap; }
	bool IsApplyOnEndOverlap() const { return EffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap; }
	bool IsRemoveOnEndOverlap() const { return RemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap; }
};
