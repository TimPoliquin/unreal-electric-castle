// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "WaitCooldownChange.generated.h"

class UAbilitySystemComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCooldownChangedSignature, float, TimeRemaining);


/**
 * 
 */
UCLASS(BlueprintType, meta=(ExposedAsyncProxy = "AsyncTask"))
class ELECTRICCASTLE_API UWaitCooldownChange : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FCooldownChangedSignature OnCooldownStart;
	UPROPERTY(BlueprintAssignable)
	FCooldownChangedSignature OnCooldownEnd;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UWaitCooldownChange* WaitForCooldownChange(
		UAbilitySystemComponent* AbilitySystemComponent,
		const FGameplayTag& InCooldownTag
	);

	UFUNCTION(BlueprintCallable)
	void EndTask();

protected:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	FGameplayTag CooldownTag;

	void CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount);
	void OnActiveEffectAdded(
		UAbilitySystemComponent* InAbilitySystemComponent,
		const FGameplayEffectSpec& SpecApplied,
		FActiveGameplayEffectHandle ActiveEffectHandle
	);
};
