#pragma once

#include "CoreMinimal.h"
#include "TimeDilationHandle.generated.h"

USTRUCT(BlueprintType)
struct FTimeDilationHandle
{
	GENERATED_BODY()
	friend class UTimeManager;

	FTimeDilationHandle() : Handle(0)
	{
	}

	bool IsValid() const { return Handle != 0; }
	bool operator==(const FTimeDilationHandle& Other) const { return Handle == Other.Handle; }
	bool operator!=(const FTimeDilationHandle& Other) const { return Handle != Other.Handle; }
	FString ToString() const { return FString::Printf(TEXT("%d"), Handle); }

private:
	void Invalidate()
	{
		Handle = 0;
	}

	UPROPERTY(VisibleInstanceOnly, Transient)
	uint32 Handle;
	UPROPERTY(VisibleInstanceOnly, Transient)
	float WorldDilation = 1.f;
	UPROPERTY(VisibleInstanceOnly, Transient)
	float PlayerDilation = 1.f;
};
