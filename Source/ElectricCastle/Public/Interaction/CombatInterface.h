// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/AttributeChangeDelegates.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

class UAbilitySystemComponent;
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAbilitySystemComponentRegisteredSignature, UAbilitySystemComponent*);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathSignature, AActor*, DeadActor);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnDamageSignature, float /* Damage Amount */);

class UNiagaraSystem;

USTRUCT(BlueprintType)
struct FTaggedMontage
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* Montage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag MontageTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag SocketTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName SocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ImpactRadius = 45.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USoundBase* ImpactSound = nullptr;
};

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API ICombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void Die() = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsDead() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	AActor* GetAvatar();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	TArray<FName> GetTargetTagsToIgnore() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	TArray<FTaggedMontage> GetAttackMontages() const;
	UE_DEPRECATED(5.7, "ICombatInterface::GetCombatSocketLocation is deprecated. Use ISocketManagerActor instead!")
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FVector GetCombatSocketLocation(const FGameplayTag& MontageTag) const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UAnimMontage* GetHitReactMontage(const FGameplayTag& HitReactTypeTag);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UNiagaraSystem* GetBloodEffect();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FTaggedMontage GetTagMontageByTag(const FGameplayTag& MontageTag);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetMinionCount() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ChangeMinionCount(const int32 Delta);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetXPReward() const;
	virtual FOnAbilitySystemComponentRegisteredSignature& GetOnAbilitySystemRegisteredDelegate() = 0;
	virtual FOnDeathSignature& GetOnDeathDelegate() = 0;
	virtual FOnDamageSignature& GetOnDamageDelegate() = 0;
	virtual void ApplyDeathImpulse(const FVector& DeathImpulse) = 0;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetActiveAbilityTag(const FGameplayTag& ActiveAbilityTag);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ClearActiveAbilityTag();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateFacingTarget(const FVector& FacingTarget);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ClearFacingTarget();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	AActor* GetWeapon() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FGameplayTag GetHitReactAbilityTagByDamageType(const FGameplayTag& DamageTypeTag) const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsHitReacting() const;
	/**
	 * 
	 * @param Actor 
	 * @return true if the provided actor implements ICombatInterface and is alive (not dead).
	 */
	static bool IsAlive(const UObject* Actor);
	/**
	 * 
	 * @param Actor
	 * @return true if the provided actor implements ICombatInterface and is dead.
	 */
	static bool IsDead(const UObject* Actor);

	static TArray<FTaggedMontage> GetAttackMontages(const UObject* Actor);

	static TArray<FName> GetTargetTagsToIgnore(const UObject* Actor);

	static int32 GetXPReward(const UObject* Actor);

	static void SetActiveAbilityTag(UObject* Actor, const FGameplayTag& ActiveAbilityTag);
	static void ClearActiveAbilityTag(UObject* Actor);
	static void UpdateFacingTarget(UObject* Actor, const FVector& FacingTarget);
	static void ClearFacingTarget(UObject* Actor);
	UE_DEPRECATED(5.7, "ICombatInterface::GetCombatSocketLocation is deprecated. Use ISocketManagerActor instead!")
	static FVector GetCombatSocketLocation(const UObject* Actor, const FGameplayTag& SocketTag);
	static AActor* GetWeapon(const UObject* Actor);
	static FGameplayTag GetHitReactAbilityTagByDamageType(const UObject* Actor, const FGameplayTag& DamageTypeTag);
	static bool IsHitReacting(const UObject* Actor);
};
