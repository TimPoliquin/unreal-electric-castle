#pragma once
#include "GameplayTagContainer.h"

#include "DialogTypes.generated.h"

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FDialogChoice
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	FGameplayTag ChoiceTag = FGameplayTag::EmptyTag;
	UPROPERTY(BlueprintReadWrite)
	FText Text = FText();
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UTexture2D> Icon = nullptr;
};
