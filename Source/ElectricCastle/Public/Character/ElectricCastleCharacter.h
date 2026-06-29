// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GenericTeamAgentInterface.h"
#include "AI/Affiliation/TeamAffiliation.h"
#include "AbilitySystem/ElectricCastleAbilitySystemInterface.h"
#include "Actor/CollidableInterface.h"
#include "Actor/Highlight/HighlightActorInterface.h"
#include "Actor/Mesh/SocketManagerActor.h"
#include "Actor/Status/StatusEffectActor.h"
#include "Interaction/CombatInterface.h"
#include "ElectricCastleCharacter.generated.h"

class UAbilityInfo;
class UCinematicHandlerComponent;
class UStatusEffectManagerComponent;
class UDissolveEffectComponent;
class UElectricCastleAttributeSet;
class UElectricCastleAbilitySystemComponent;
class UPassiveNiagaraComponent;
class UDebuffNiagaraComponent;
class UNiagaraSystem;
struct FGameplayTag;
struct FElectricCastleAbilityInfo;
class UGameplayAbility;
class UGameplayEffect;
class UAttributeSet;
class UAbilitySystemComponent;
struct FElectricCastleAbilitySpec;

UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API AElectricCastleCharacter : public ACharacter,
                                                    public IAbilitySystemInterface,
                                                    public IGenericTeamAgentInterface,
                                                    public IElectricCastleAbilitySystemInterface,
                                                    public IHighlightActorInterface,
                                                    public ICombatInterface,
                                                    public ICollidableInterface,
                                                    public ISocketManagerActor,
                                                    public IStatusEffectActor
{
	GENERATED_BODY()

public:
	AElectricCastleCharacter();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual UElectricCastleAttributeSet* GetAttributeSet() const { return nullptr; }

	virtual void SetActorHiddenInGame(bool bNewHidden) override;

	virtual float TakeDamage(
		float DamageAmount,
		const struct FDamageEvent& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser
	) override;

	/** Start GenericTeamAgentInterface **/
	virtual FGenericTeamId GetGenericTeamId() const override;
	/** End GenericTeamAgentInterface **/

	/** Start HighlightActorInterface **/
	virtual UHighlightComponent* GetHighlightComponent_Implementation() const override;
	virtual void GetHighlightMeshes_Implementation(TArray<UMeshComponent*>& OutHighlightMeshes) override;
	/** End HighlightActorInterface **/

	/** Combat Interface **/
	virtual AActor* GetAvatar_Implementation() override;
	virtual TArray<FTaggedMontage> GetAttackMontages_Implementation() const override;
	virtual FTaggedMontage GetTagMontageByTag_Implementation(const FGameplayTag& MontageTag) override;
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) const override;
	virtual void Die() override;
	virtual bool IsDead_Implementation() const override;
	virtual UNiagaraSystem* GetBloodEffect_Implementation() override;
	virtual int32 GetXPReward_Implementation() const override;
	virtual AActor* GetWeapon_Implementation() const override;
	virtual void HandleTakeDamage(const float IncomingDamage, const FEffectProperties& Props) override;
	virtual void HandleDodge_Implementation() override;


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
	}

	virtual void ApplyDeathImpulse(const FVector& DeathImpulse) override;

	virtual void SetActiveAbilityTag_Implementation(const FGameplayTag& InActiveAbilityTag) override
	{
		ActiveAbilityTag = InActiveAbilityTag;
	}

	virtual void ClearActiveAbilityTag_Implementation() override
	{
		ActiveAbilityTag = FGameplayTag::EmptyTag;
	}

	/** Combat Interface End **/

	/** Start ICollidableInterface **/
	virtual UShapeComponent* GetPrimaryCollisionComponent() const override;
	/** End ICollidableInterface **/

	/** Start ISocketManagerActor **/
	virtual USocketManagerComponent* GetSocketManagerComponent_Implementation() const override;
	/** End ISocketManagerActor **/

	/** Start IStatusEffectActor **/
	virtual UStatusEffectManagerComponent* GetStatusEffectManagerComponent_Implementation() const override
	{
		return StatusEffectManagerComponent;
	}

	/** End IStatusEffectActor **/


	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath();
	UFUNCTION(BlueprintImplementableEvent)
	void HandleDeathLocal();

protected:
	virtual void BeginPlay() override;
	virtual void SetActorTickEnabled(bool bEnabled) override;

	virtual void InitializeAbilityActorInfo()
	{
	};

	UPROPERTY()
	TObjectPtr<UElectricCastleAbilitySystemComponent> AbilitySystemComponent;

	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> Attributes, const float Level) const;

	void AddCharacterAbilities();
	virtual void HandleMovementSpeedAttributeChanged(const FOnAttributeChangeData& OnAttributeChangeData);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float BaseWalkSpeed = 250.f;

	/** Dissolve Effect */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UDissolveEffectComponent> CharacterDissolveComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USocketManagerComponent> SocketManagerComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStatusEffectManagerComponent> StatusEffectManagerComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCinematicHandlerComponent> CinematicHandlerComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UHighlightComponent> HighlightComponent;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Dissolve")
	void Dissolve() const;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	ETeamAffiliation TeamAffiliation = ETeamAffiliation::None;
	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;
	UPROPERTY(EditAnywhere, Category = "Combat", meta=(Categories="Effect.HitReact"))
	TMap<FGameplayTag, TObjectPtr<UAnimMontage>> HitReactionMontageByMontageTag;
	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FTaggedMontage> AttackMontages;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties|Status Effects|Stagger")
	float StaggerLaunchForce = 250.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Properties|Status Effects|Stagger")
	float StaggerLaunchUpwardForce = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UNiagaraSystem> BloodEffect;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	TObjectPtr<USoundBase> DeathSound;

	UPROPERTY(VisibleAnywhere, Category="Combat|Passive")
	TObjectPtr<USceneComponent> EffectAttachComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Abilities")
	TArray<TObjectPtr<UAbilityInfo>> Abilities;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_ActiveAbilityTag, Category = "Combat")
	FGameplayTag ActiveAbilityTag;
	UFUNCTION()
	virtual void OnRep_ActiveAbilityTag()
	{
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TArray<FGameplayTag> StatusEffectTags;

	UFUNCTION()
	virtual void OnRep_StatusEffectTags()
	{
	}

	UFUNCTION(BlueprintNativeEvent)
	void OnStatusStaggeredAdded();
	UFUNCTION(BlueprintNativeEvent)
	void OnStatusStaggeredRemoved();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	float GetStaggerLaunchForce() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	float GetStaggerLaunchUpwardForce() const;

	UFUNCTION(BlueprintNativeEvent)
	void OnAbilitySystemReady(UElectricCastleAbilitySystemComponent* InAbilitySystemComponent);
	UFUNCTION(BlueprintNativeEvent)
	void OnEffectChange_LightningDamage(FGameplayTag LightningDamageTag, int Count);
	UFUNCTION(BlueprintNativeEvent)
	void OnEffectAdd_LightningDamage();
	UFUNCTION(BlueprintNativeEvent)
	void OnEffectRemove_LightningDamage();

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
	void OnEffectChange_Staggered(FGameplayTag StaggeredTag, int Count);
	UFUNCTION()
	void RegisterStatusEffectTags(UElectricCastleAbilitySystemComponent* InAbilitySystemComponent);
};
