// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/ElectricCastleGameplayAbility.h"
#include "FormChangeGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UFormChangeGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UFormChangeGameplayAbility();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool CanChangeForms(const FGameplayTag NewFormTag) const;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Categories="Player.Form"))
	FGameplayTag FormTag;
};
