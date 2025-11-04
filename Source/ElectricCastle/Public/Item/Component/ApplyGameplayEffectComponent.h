// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Item/ElectricCastleGameplayEffectTypes.h"
#include "Components/ActorComponent.h"
#include "ApplyGameplayEffectComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API UApplyGameplayEffectComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UApplyGameplayEffectComponent();

	UPROPERTY(BlueprintAssignable)
	FOnPlayerOverlapSignature OnPlayerOverlapStart;
	UPROPERTY(BlueprintAssignable)
	FOnPlayerOverlapSignature OnPlayerOverlapEnd;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool CheckPreRequisites(AActor* PickupActor) const;
	UFUNCTION(BlueprintCallable)
	void OnOverlap(AActor* TargetActor);
	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(AActor* TargetActor);
	void SetDefaults(
		const TArray<FGameplayEffectConfig>& InEffectConfig,
		const bool InDestroyOnEffectApplication,
		const TArray<FName>& InApplyToTags
	);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsAffectingActor(AActor* Actor) const;
	int32 GetActorLevel() const { return ActorLevel; }
	void SetActorLevel(const int32 Level) { ActorLevel = Level; }
	void SetDestroyOnEffectApplication(const bool bInDestroyOnEffectApplication) { bDestroyOnEffectApplication = bInDestroyOnEffectApplication; }
	void SetApplyToTags(const TArray<FName>& InApplyToTags) { ApplyToTags = InApplyToTags; }

protected:
	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(AActor* TargetActor, const FGameplayEffectConfig& GameplayEffectConfig);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	TArray<FGameplayEffectConfig> GameplayEffectConfigs;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	bool bDestroyOnEffectApplication = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	TArray<FName> ApplyToTags;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Applied Effects")
	int32 ActorLevel = 1.f;

private:
	UPROPERTY()
	TMap<FActiveGameplayEffectHandle, UAbilitySystemComponent*> ActiveEffectHandles;
	void RemoveEffectsFromTarget(AActor* TargetActor);
};
