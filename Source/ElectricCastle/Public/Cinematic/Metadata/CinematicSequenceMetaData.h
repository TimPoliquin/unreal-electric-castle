// Copyright Alien Shores

#pragma once

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Cinematic/CinematicEvents.h"
#include "CinematicSequenceMetaData.generated.h"

UCLASS(BlueprintType, EditInlineNew, CollapseCategories)
class ELECTRICCASTLE_API UCinematicSequenceMetaData : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cinematic|Metadata", meta = (Categories = "Cinematic.Type"))
	FGameplayTagContainer TypeTags = FGameplayTagContainer::EmptyContainer;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cinematic|Metadata", meta=(Categories="Cinematic.Reaction.Trigger"))
	FGameplayTagContainer ReactionTriggerTags = FGameplayTagContainer::EmptyContainer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cinematic|Metadata")
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cinematic|Metadata")
	bool bSkippable = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cinematic|Metadata")
	ECinematicPlayerRelocationMode PlayerRelocationMode = ECinematicPlayerRelocationMode::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cinematic|Metadata", meta=(EditCondition="PlayerRelocationMode == ECinematicPlayerRelocationMode::CinematicOverride", EditConditionHides))
	FVector RelocatePlayerLocation = FVector::ZeroVector;

public:
	UFUNCTION(BlueprintCallable)
	void CopyFrom(const UCinematicSequenceMetaData* Source);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FGameplayTagContainer GetTypeTags() const { return TypeTags; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FGameplayTagContainer GetReactionTriggerTags() const { return ReactionTriggerTags; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FString GetDescription() const { return Description; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsSkippable() const { return bSkippable; }

	UFUNCTION(BlueprintPure, Category = "Cinematic|Metadata")
	bool HasTypeTag(const FGameplayTag& Tag) const { return TypeTags.HasTag(Tag); }

	UFUNCTION(BlueprintPure, Category = "Cinematic|Metadata")
	bool HasAnyTypeTags(const FGameplayTagContainer& Tags) const { return TypeTags.HasAny(Tags); }

	UFUNCTION(BlueprintPure, Category = "Cinematic|Metadata")
	bool HasReactionTriggerTag(const FGameplayTag& Tag) const { return ReactionTriggerTags.HasTag(Tag); }

	UFUNCTION(BlueprintPure, Category = "Cinematic|Metadata")
	bool HasAnyReactionTriggerTags(const FGameplayTagContainer& Tags) const { return ReactionTriggerTags.HasAny(Tags); }

	UFUNCTION(BlueprintPure, Category = "Cinematic|Metadata")
	ECinematicPlayerRelocationMode GetPlayerRelocationMode() const { return PlayerRelocationMode; }

	UFUNCTION(BlueprintPure, Category = "Cinematic|Metadata")
	FVector GetPlayerRelocationLocation() const { return RelocatePlayerLocation; }
};
