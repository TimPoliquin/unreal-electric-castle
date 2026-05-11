// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AI/Engagement/EngagementTypes.h"
#include "UObject/Object.h"
#include "AbilityEngagementConfig.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, DefaultToInstanced, EditInlineNew)
class ELECTRICCASTLE_API UAbilityEngagementConfig : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsAbilitySupportedInRange(const EEngagementRange InEngagementRange) const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsAbilitySupportedInModes(const TArray<EEngagementAbilityMode>& InEngagementAbilityModes) const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<EEngagementRange> SupportedEngagementRanges;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<EEngagementAbilityMode> SupportedEngagementAbilityModes;
};
