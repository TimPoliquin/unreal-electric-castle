// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "StatusEffectEvents.h"
#include "Components/ActorComponent.h"
#include "StatusEffectManagerComponent.generated.h"

class UNiagaraSystem;
struct FStatusEffectFXInstance;
class UStatusEffectBarWidget;
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
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UStatusEffectBarWidget* GetStatusEffectBarWidget() const;
	void SetStatusEffectBarWidget(UStatusEffectBarWidget* StatusEffectBarWidget);
	UFUNCTION(BlueprintCallable)
	void AddStatusEffectNiagaraSystem(const FGameplayTag& InStatusEffectTag, UNiagaraSystem* NiagaraSystem, const FStatusEffectNiagaraConfig& Config);
	UFUNCTION(BlueprintCallable)
	void RemoveStatusEffectNiagaraSystem(const FGameplayTag& InStatusEffectTag);

	UPROPERTY(BlueprintAssignable)
	FOnStatusEffectAddedSignature OnStatusEffectAddedDelegate;
	UPROPERTY(BlueprintAssignable)
	FOnStatusEffectRemovedSignature OnStatusEffectRemovedDelegate;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties")
	bool bDebug = false;

private:
	UFUNCTION()
	void AddStatusEffectListeners(UElectricCastleAbilitySystemComponent* AbilitySystemComponent);
	UFUNCTION()
	void OnEffectAdded(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle) const;
	UFUNCTION()
	void OnEffectRemoved(const FActiveGameplayEffect& ActiveGameplayEffect) const;
	UFUNCTION()
	void HandleOwnerDeath(AActor* DeadActor);
	UPROPERTY()
	TObjectPtr<UStatusEffectBarWidget> StatusEffectBarWidget;
	UPROPERTY()
	TArray<FStatusEffectFXInstance> StatusEffectFXInstances;
};
