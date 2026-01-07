#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StatusEffectTypes.generated.h"

class UElectricCastleAbilitySystemComponent;

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
	float Duration = 0.f; 
	UPROPERTY(BlueprintReadWrite)
	float TimeRemaining = 0.f;
	UPROPERTY(BlueprintReadWrite)
	bool bIsInfinite = false;
	
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