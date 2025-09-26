// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"
#include "EffectAreaActor.generated.h"

class UApplyGameplayEffectComponent;
class UAbilitySystemComponent;
class UGameplayEffect;

UCLASS()
class ELECTRICCASTLE_API AEffectAreaActor : public AActor
{
	GENERATED_BODY()

public:
	AEffectAreaActor();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UApplyGameplayEffectComponent> EffectComponent;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	bool bPlaySpawnEffect = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawn")
	TObjectPtr<USoundBase> SpawnSound;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void PlaySpawnEffect();

private:
	UPROPERTY()
	TMap<FActiveGameplayEffectHandle, UAbilitySystemComponent*> ActiveEffectHandles;
};
