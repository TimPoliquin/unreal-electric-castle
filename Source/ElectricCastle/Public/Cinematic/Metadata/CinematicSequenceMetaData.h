// Copyright Alien Shores

#pragma once

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "CinematicSequenceMetaData.generated.h"

UCLASS(BlueprintType, EditInlineNew, CollapseCategories)
class ELECTRICCASTLE_API UCinematicSequenceMetaData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cinematic|Metadata", meta = (Categories = "Cinematic.Type"))
	FGameplayTagContainer TypeTags = FGameplayTagContainer::EmptyContainer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cinematic|Metadata")
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cinematic|Metadata")
	bool bSkippable = true;

	UFUNCTION(BlueprintPure, Category = "Cinematic|Metadata")
	bool HasTypeTag(const FGameplayTag& Tag) const { return TypeTags.HasTag(Tag); }

	UFUNCTION(BlueprintPure, Category = "Cinematic|Metadata")
	bool HasAnyTypeTags(const FGameplayTagContainer& Tags) const { return TypeTags.HasAny(Tags); }
};
