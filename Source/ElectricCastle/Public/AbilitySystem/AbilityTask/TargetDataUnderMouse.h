// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "TargetDataUnderMouse.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FMouseTargetDataSignature,
	const FGameplayAbilityTargetDataHandle&,
	DataHandle
);

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UTargetDataUnderMouse : public UAbilityTask
{
	GENERATED_BODY()

public:
	UFUNCTION(
		BlueprintCallable,
		Category = "Ability|Tasks",
		meta=(
			HidePin="OwningAbility",
			DefaultToSelf="OwningAbility",
			BlueprintInternalUseOnly = true,
			DisplayName="TargetDataUnderMouse"
		)
	)
	static UTargetDataUnderMouse* CreateTargetDataUnderMouse(
		UGameplayAbility* OwningAbility,
		bool bPreferLivingActors,
		float SweepRadius = 5.f,
		float SweepDistance = 10000,
		bool bDebug = false
	);
	bool bPreferLivingActors = true;
	float SweepRadius = 5.f;
	float SweepDistance = 10000;
	bool bDebug = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn=true))
	TArray<AActor*> IgnoreActors;
	UPROPERTY(BlueprintAssignable)
	FMouseTargetDataSignature HasMouseTarget;
	UPROPERTY(BlueprintAssignable)
	FMouseTargetDataSignature HasNoTarget;

protected:
	void OnTargetDataReplicatedCallback(
		const FGameplayAbilityTargetDataHandle& DataHandle,
		FGameplayTag GameplayTag
	) const;
	virtual void Activate() override;

private:
	void SendMouseCursorDataToServer() const;
	bool ShouldTrySphereTrace(const FHitResult& HitResult) const;
};
