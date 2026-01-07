// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Components/ActorComponent.h"
#include "StatusEffectFXComponent.generated.h"


class UNiagaraComponent;
class UNiagaraSystem;

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FStatusEffectFXInstance
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	FGameplayTag StatusEffectTag;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UNiagaraComponent> FXInstance;

	bool IsValid() const
	{
		return StatusEffectTag.IsValid();
	}
};

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
	bool HasFXInstanceByTag(const FGameplayTag& StatusEffectTag) const;
	FStatusEffectFXInstance FindFXInstanceByTag(const FGameplayTag& StatusEffectTag) const;
	UFUNCTION(Client, Reliable)
	void SpawnStatusEffectFX(const FGameplayTag& InStatusEffectTag);
	UFUNCTION(Client, Reliable)
	void DestroyStatusEffectFX(const FGameplayTag& InStatusEffectTag);
	UFUNCTION()
	void OnStatusEffectTagCountChanged(FGameplayTag InStatusEffectTag, int32 InCount);
	void AddStatusEffectTagListeners(UAbilitySystemComponent* AbilitySystemComponent);
	UPROPERTY()
	TArray<FStatusEffectFXInstance> StatusEffectFXInstances;
};
