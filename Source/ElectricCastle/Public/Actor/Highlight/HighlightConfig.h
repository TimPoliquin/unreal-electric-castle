// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "HighlightTypes.h"

#include "HighlightConfig.generated.h"

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FHighlightConfigRow
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 HighlightCode = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor HighlightColor = FLinearColor::White;

	bool IsValid() const
	{
		return HighlightCode > 0;
	}
};

/**
 * 
 */
UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class ELECTRICCASTLE_API UHighlightConfig : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FHighlightConfigRow GetHighlightConfigByHighlightType(const EHighlightType HighlightType) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<EHighlightType, FHighlightConfigRow> HighlightTypes;
};
