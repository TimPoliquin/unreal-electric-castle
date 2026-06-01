#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StatusEffectTypes.generated.h"

class UNiagaraComponent;
class UElectricCastleAbilitySystemComponent;

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FStatusEffectDuration
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	float Duration = 0.f;
	UPROPERTY(BlueprintReadWrite)
	float DurationRemaining = 0.f;
	UPROPERTY(BlueprintReadWrite)
	bool bIsInfinite = false;
};

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FOnStatusEffectAddedPayload
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> AffectedActor;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UElectricCastleAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer EffectTags = FGameplayTag::EmptyTag.GetSingleTagContainer();
	UPROPERTY(BlueprintReadWrite)
	FStatusEffectDuration Duration;

	bool IsValid() const
	{
		return AffectedActor && AbilitySystemComponent && EffectTags.IsValid();
	}
};

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FOnStatusEffectRemovedPayload
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> AffectedActor;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UElectricCastleAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer EffectTags = FGameplayTag::EmptyTag.GetSingleTagContainer();
};

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FStatusEffectFXInstance
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	FGameplayTag StatusEffectTag;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UNiagaraComponent> FXInstance;

	bool IsValid() const
	{
		return StatusEffectTag.IsValid();
	}
};

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FStatusEffectNiagaraConfig
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName AttachPoint = NAME_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Location = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator Rotation = FRotator::ZeroRotator;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Scale = FVector::OneVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EAttachLocation::Type> LocationType = EAttachLocation::Type::SnapToTarget;
};
