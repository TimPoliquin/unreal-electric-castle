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
	FText Text;
	UPROPERTY(BlueprintReadWrite)
	UTexture2D* Icon;
};
