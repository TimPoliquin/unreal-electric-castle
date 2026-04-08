// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "StatusEffectEvents.h"
#include "Components/ActorComponent.h"
#include "StatusEffectManagerComponent.generated.h"

struct FActiveGameplayEffectHandle;
struct FGameplayEffectSpec;
class UAbilitySystemComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API UStatusEffectManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UStatusEffectManagerComponent();
	UPROPERTY(BlueprintAssignable)
	FOnStatusEffectAddedSignature OnStatusEffectAddedDelegate;
	UPROPERTY(BlueprintAssignable)
	FOnStatusEffectRemovedSignature OnStatusEffectRemovedDelegate;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties")
	bool bDebug = false;

private:
	UFUNCTION()
	void AddStatusEffectListeners(UElectricCastleAbilitySystemComponent* AbilitySystemComponent);
	UFUNCTION()
	void OnEffectAdded(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle) const;
	UFUNCTION()
	void OnEffectRemoved(const FActiveGameplayEffect& ActiveGameplayEffect) const;
};
