// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Components/ActorComponent.h"
#include "StatusEffectFXComponent.generated.h"


class UNiagaraComponent;
class UNiagaraSystem;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API UStatusEffectFXComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UStatusEffectFXComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
	TMap<FGameplayTag, TObjectPtr<UNiagaraSystem>> StatusEffectFX;

private:
	void SpawnStatusEffectFX(const FGameplayTag& StatusEffectTag);
	void DestroyStatusEffectFX(const FGameplayTag& StatusEffectTag);
	UFUNCTION()
	void OnStatusEffectTagCountChanged(FGameplayTag InStatusEffectTag, int32 InCount);
	void AddStatusEffectTagListeners(UAbilitySystemComponent* AbilitySystemComponent);
	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UNiagaraComponent>> StatusEffectFXInstances;
};
