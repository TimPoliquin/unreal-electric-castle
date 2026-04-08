// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "TimeDilationHandle.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TimeManager.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UTimeManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static UTimeManager* Get(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable)
	FTimeDilationHandle StartTimeDilation(const UObject* WorldContextObject, const float WorldTimeDilation, const float PlayerTimeDilation);
	UFUNCTION(BlueprintCallable)
	void ClearTimeDilation(const UObject* WorldContextObject, UPARAM(ref) FTimeDilationHandle& Handle);

private:
	UPROPERTY(VisibleInstanceOnly)
	uint32 HandleIndex = 0;
	UPROPERTY(VisibleInstanceOnly)
	TArray<FTimeDilationHandle> TimeDilationHandles;
};
