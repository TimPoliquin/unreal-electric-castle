// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/ElectricCastleAbilitySystemComponent.h"
#include "PassiveNiagaraComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API UPassiveNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPassiveNiagaraComponent();

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag PassiveSpellTag;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnPassiveActivate(const FGameplayTag& AbilityTag, const bool bActivate);
	void ActivateIfAbilityIsAlreadyActive(UElectricCastleAbilitySystemComponent* AbilitySystemComponent);
};
