// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "PlayerFormAttributeModifiers.generated.h"

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FPlayerFormAttributeModifierEntry
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Categories="Attributes"))
	FGameplayTag AttributeTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0))
	float Modifier = 1.f;

	bool IsValid() const
	{
		return AttributeTag.IsValid();
	}
};

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UPlayerFormAttributeModifiers : public UDataAsset
{
	GENERATED_BODY()

public:
	float GetAttributeModifier(const FGameplayTag& AttributeTag) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FPlayerFormAttributeModifierEntry> AttributeModifiers;
};
