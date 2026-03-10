#pragma once

#include "CoreMinimal.h"
#include "HighlightTypes.generated.h"

UENUM(BlueprintType)
enum class EHighlightType : uint8
{
	None,
	Friendly,
	Enemy,
	Count
};

USTRUCT()
struct FHighlightContext
{
	GENERATED_BODY()

	FHighlightContext()
	{
	}

	UPROPERTY()
	TObjectPtr<AActor> LastActor;
	UPROPERTY()
	TObjectPtr<AActor> CurrentActor;

	void Track(AActor* Actor);
	void Clear();
	bool HasCurrentTarget() const;
	bool IsDifferentPtr() const;
	void HighlightCurrent() const;
	void UnHighlightCurrent() const;
	void UnHighlightLast() const;
};
