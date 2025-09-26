// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/AttributeChangeDelegates.h"
#include "AbilitySystem/ElectricCastleAbilitySystemInterface.h"
#include "Actor/CollidableInterface.h"
#include "Interaction/CombatInterface.h"
#include "ElectricCastleCharacter.generated.h"

class UDissolveEffectComponent;
class UElectricCastleAttributeSet;
class UElectricCastleAbilitySystemComponent;
class UPassiveNiagaraComponent;
class UDebuffNiagaraComponent;
class UNiagaraSystem;
struct FGameplayTag;
class UGameplayAbility;
class UGameplayEffect;
class UAttributeSet;
class UAbilitySystemComponent;

UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API AElectricCastleCharacter : public ACharacter, public IAbilitySystemInterface, public IElectricCastleAbilitySystemInterface, public ICombatInterface,
                                                    public ICollidableInterface
{
	GENERATED_BODY()

public:
	AElectricCastleCharacter();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual UElectricCastleAttributeSet* GetAttributeSet() const { return nullptr; }

	virtual float TakeDamage(
		float DamageAmount,
		const struct FDamageEvent& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser
	) override;

	/** Combat Interface **/
	virtual AActor* GetAvatar_Implementation() override;
	virtual UAnimMontage* GetHitReactMontage_Implementation(const FGameplayTag& HitReactTypeTag) override;
	virtual TArray<FTaggedMontage> GetAttackMontages_Implementation() const override;
	virtual FTaggedMontage GetTagMontageByTag_Implementation(const FGameplayTag& MontageTag) override;
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) const override;
	virtual void Die() override;
	virtual bool IsDead_Implementation() const override;
	virtual UNiagaraSystem* GetBloodEffect_Implementation() override;
	virtual int32 GetXPReward_Implementation() const override;

	virtual FOnDeathSignature& GetOnDeathDelegate() override
	{
		return OnDeathDelegate;
	}

	virtual FOnAbilitySystemComponentRegisteredSignature& GetOnAbilitySystemRegisteredDelegate() override
	{
		return OnAbilitySystemComponentRegisteredDelegate;
	}

	virtual FOnDamageSignature& GetOnDamageDelegate() override
	{
		return OnDamageDelegate;
	}

	virtual TArray<FName> GetTargetTagsToIgnore_Implementation() const override
	{
		return TArray<FName>();
	};
	virtual int32 GetMinionCount_Implementation() const override;
	virtual void ChangeMinionCount_Implementation(const int32 Delta) override;
	virtual void ApplyDeathImpulse(const FVector& DeathImpulse) override;

	virtual USkeletalMeshComponent* GetWeapon_Implementation() const override
	{
		return nullptr;
	}

	virtual void SetActiveAbilityTag_Implementation(const FGameplayTag& InActiveAbilityTag) override
	{
		ActiveAbilityTag = InActiveAbilityTag;
	}

	virtual void ClearActiveAbilityTag_Implementation() override
	{
		ActiveAbilityTag = FGameplayTag::EmptyTag;
	}

	virtual FGameplayTag
	GetHitReactAbilityTagByDamageType_Implementation(const FGameplayTag& DamageTypeTag) const override;

	/** Combat Interface End **/

	/** Start ICollidableInterface **/
	virtual UShapeComponent* GetPrimaryCollisionComponent() const override;
	/** End ICollidableInterface **/

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath();

protected:
	virtual void BeginPlay() override;

	virtual void InitializeAbilityActorInfo()
	{
	};

	UPROPERTY()
	TObjectPtr<UElectricCastleAbilitySystemComponent> AbilitySystemComponent;

	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> Attributes, const float Level) const;

	void AddCharacterAbilities();
	UFUNCTION()
	virtual void OnHitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	virtual bool IsHitReacting_Implementation() const override;
	UFUNCTION(BlueprintCallable)
	bool IsShocked() const;
	UFUNCTION(BlueprintCallable)
	bool IsBurned() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float BaseWalkSpeed = 250.f;

	/** Dissolve Effect */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UDissolveEffectComponent> CharacterDissolveComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UDissolveEffectComponent> WeaponDissolveComponent;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Dissolve")
	void Dissolve() const;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat")
	TMap<FGameplayTag, FGameplayTag> HitReactionsByDamageType;
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bHitReacting;
	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;
	UPROPERTY(EditAnywhere, Category = "Combat", meta=(Categories="Effect.HitReact"))
	TMap<FGameplayTag, TObjectPtr<UAnimMontage>> HitReactionMontageByMontageTag;
	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FTaggedMontage> AttackMontages;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UNiagaraSystem> BloodEffect;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	TObjectPtr<USoundBase> DeathSound;

	UPROPERTY(VisibleAnywhere, Category="Combat|Debuff")
	TObjectPtr<UDebuffNiagaraComponent> BurnDebuffComponent;
	UPROPERTY(VisibleAnywhere, Category="Combat|Debuff")
	TObjectPtr<UDebuffNiagaraComponent> ShockDebuffComponent;
	UPROPERTY(VisibleAnywhere, Category="Combat|Passive")
	TObjectPtr<USceneComponent> EffectAttachComponent;
	UPROPERTY(VisibleAnywhere, Category = "Combat|Passive")
	TObjectPtr<UPassiveNiagaraComponent> HaloOfProtectionNiagaraComponent;
	UPROPERTY(VisibleAnywhere, Category = "Combat|Passive")
	TObjectPtr<UPassiveNiagaraComponent> LifeSiphonNiagaraComponent;
	UPROPERTY(VisibleAnywhere, Category = "Combat|Passive")
	TObjectPtr<UPassiveNiagaraComponent> ManaSiphonNiagaraComponent;


	/** Minions **/
	int32 MinionCount = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_ActiveAbilityTag, Category = "Combat")
	FGameplayTag ActiveAbilityTag;
	UFUNCTION()
	virtual void OnRep_ActiveAbilityTag()
	{
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_StatusEffectTags, Category = "Combat")
	TArray<FGameplayTag> StatusEffectTags;

	UFUNCTION()
	virtual void OnRep_StatusEffectTags()
	{
	}

	virtual void OnStatusShockAdded()
	{
	};

	virtual void OnStatusShockRemoved()
	{
	};

	virtual void OnStatusBurnAdded()
	{
	};

	virtual void OnStatusBurnRemoved()
	{
	};

	UFUNCTION(BlueprintNativeEvent)
	void OnAbilitySystemReady(UElectricCastleAbilitySystemComponent* InAbilitySystemComponent);

private:
	bool bDead = false;
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartingAbilities;
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartingPassiveAbilities;
	FOnAbilitySystemComponentRegisteredSignature OnAbilitySystemComponentRegisteredDelegate;
	FOnDeathSignature OnDeathDelegate;
	FOnDamageSignature OnDamageDelegate;
	void Dissolve(
		UMeshComponent* InMesh,
		UMaterialInstance* MaterialInstance,
		void (AElectricCastleCharacter::*Callback)(UMaterialInstanceDynamic*)
	);
	UFUNCTION()
	void OnDebuffTypeBurnChanged(FGameplayTag GameplayTag, int StackCount);
	UFUNCTION()
	void OnDebuffTypeShockChanged(FGameplayTag StunTag, int32 Count);
	UFUNCTION()
	void RegisterStatusEffectTags(UAbilitySystemComponent* InAbilitySystemComponent);
};
