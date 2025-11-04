// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"
#include "DebuffNiagaraComponent.generated.h"


class UAbilitySystemComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API UDebuffNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDebuffNiagaraComponent();

	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere)
	FGameplayTag DebuffTag;

private:
	UFUNCTION()
	void OnDebuffTagChanged(FGameplayTag GameplayTag, int StackCount);
	UFUNCTION()
	void OnOwnerDeath(AActor* Actor);
};
