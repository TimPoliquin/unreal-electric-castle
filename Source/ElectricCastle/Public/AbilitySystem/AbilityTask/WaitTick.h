// Copyright Alien Shores

#pragma once

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "WaitTick.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTickTaskDelegate, float, DeltaTime);

/**
 * Task for abilities that supply tick and its' delta time.
 */
UCLASS(BlueprintType, meta=(ExposedAsyncProxy = AsyncTask))
class ELECTRICCASTLE_API UWaitTick : public UAbilityTask
{
	GENERATED_BODY()

	UPROPERTY(BlueprintAssignable)
	FOnTickTaskDelegate OnTick;

public:
	UWaitTick(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UWaitTick* WaitTick(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName);

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;
};
