// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Attribute/ElectricCastleAttributeTypes.h"
#include "ElectricCastleAttributeSet.generated.h"

class UAbilitySystemComponent;


/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UElectricCastleAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UElectricCastleAttributeSet();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsFullHealth() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsFullMana() const;

	void InitializeDefaultAttributes(const int32 Level = 1);
	void ToSaveData(FAttributeSetSaveData& SaveData) const;
	void FromSaveData(const FAttributeSetSaveData& SaveData);

	TMap<FGameplayTag, TStaticFuncPtr<FGameplayAttribute()>> TagsToAttributes;

	/**
	 * Primary Attributes
	 */
	// Max Health
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Primary Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UElectricCastleAttributeSet, MaxHealth);
	UFUNCTION()
	FORCEINLINE void OnRep_MaxHealth(const FGameplayAttributeData& OldValue) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UElectricCastleAttributeSet, MaxHealth, OldValue);
	}

	// Max Mana
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Primary Attributes")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UElectricCastleAttributeSet, MaxMana);
	UFUNCTION()
	FORCEINLINE void OnRep_MaxMana(const FGameplayAttributeData& OldValue) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UElectricCastleAttributeSet, MaxMana, OldValue);
	}

	// Strength
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Strength, Category = "Primary Attributes")
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(UElectricCastleAttributeSet, Strength);
	UFUNCTION()
	FORCEINLINE void OnRep_Strength(const FGameplayAttributeData& OldStrength) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UElectricCastleAttributeSet, Strength, OldStrength);
	}

	// Agility
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Agility, Category = "Primary Attributes")
	FGameplayAttributeData Agility;
	ATTRIBUTE_ACCESSORS(UElectricCastleAttributeSet, Agility);
	UFUNCTION()
	FORCEINLINE void OnRep_Agility(const FGameplayAttributeData& OldValue) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UElectricCastleAttributeSet, Agility, OldValue);
	}

	// Constitution
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Constitution, Category = "Primary Attributes")
	FGameplayAttributeData Constitution;
	ATTRIBUTE_ACCESSORS(UElectricCastleAttributeSet, Constitution);
	UFUNCTION()
	FORCEINLINE void OnRep_Constitution(const FGameplayAttributeData& OldConstitution) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UElectricCastleAttributeSet, Constitution, OldConstitution);
	}

	// Intelligence
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Intelligence, Category = "Primary Attributes")
	FGameplayAttributeData Intelligence;
	ATTRIBUTE_ACCESSORS(UElectricCastleAttributeSet, Intelligence);
	UFUNCTION()
	FORCEINLINE void OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UElectricCastleAttributeSet, Intelligence, OldIntelligence);
	}

	// Wisdom
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Wisdom, Category = "Primary Attributes")
	FGameplayAttributeData Wisdom;
	ATTRIBUTE_ACCESSORS(UElectricCastleAttributeSet, Wisdom);
	UFUNCTION()
	FORCEINLINE void OnRep_Wisdom(const FGameplayAttributeData& OldValue) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UElectricCastleAttributeSet, Wisdom, OldValue);
	}

	/**
	 * Secondary Attributes
	 */
	// Attack Power - Strength + Weapon Attack Power
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_AttackPower, Category="Secondary Attributes")
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(UElectricCastleAttributeSet, AttackPower);
	UFUNCTION()
	FORCEINLINE void OnRep_AttackPower(const FGameplayAttributeData& OldValue) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UElectricCastleAttributeSet, AttackPower, OldValue);
	}

	// Magic Power - Strength + Weapon Magic Power
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MagicPower, Category="Secondary Attributes")
	FGameplayAttributeData MagicPower;
	ATTRIBUTE_ACCESSORS(UElectricCastleAttributeSet, MagicPower);
	UFUNCTION()
	FORCEINLINE void OnRep_MagicPower(const FGameplayAttributeData& OldValue) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UElectricCastleAttributeSet, MagicPower, OldValue);
	}

	// Defense - Constitution + Armor
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Defense, Category = "Secondary Attributes")
	FGameplayAttributeData Defense;
	ATTRIBUTE_ACCESSORS(UElectricCastleAttributeSet, Defense);
	UFUNCTION()
	FORCEINLINE void OnRep_Defense(const FGameplayAttributeData& OldValue) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UElectricCastleAttributeSet, Defense, OldValue);
	}

	// Magic Defense - Wisdom + Armor
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MagicDefense, Category="Secondary Attributes")
	FGameplayAttributeData MagicDefense;
	ATTRIBUTE_ACCESSORS(UElectricCastleAttributeSet, MagicDefense);
	UFUNCTION()
	FORCEINLINE void OnRep_MagicDefense(const FGameplayAttributeData& OldValue) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UElectricCastleAttributeSet, MagicDefense, OldValue);
	}

	// Hit Chance - 75 + 1/4 Agility
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_HitChance, Category="Secondary Attributes")
	FGameplayAttributeData HitChance;
	ATTRIBUTE_ACCESSORS(UElectricCastleAttributeSet, HitChance);
	UFUNCTION()
	FORCEINLINE void OnRep_HitChance(const FGameplayAttributeData& OldValue) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UElectricCastleAttributeSet, HitChance, OldValue);
	}

	// Evade Chance - Armor + 1/4 Agility
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_EvadeChance, Category="Secondary Attributes")
	FGameplayAttributeData EvadeChance;
	ATTRIBUTE_ACCESSORS(UElectricCastleAttributeSet, EvadeChance);
	UFUNCTION()
	FORCEINLINE void OnRep_EvadeChance(const FGameplayAttributeData& OldValue) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UElectricCastleAttributeSet, EvadeChance, OldValue);
	}

	// Critical Hit Chance - 5 + 1/5 Agility
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_EvadeChance, Category="Secondary Attributes")
	FGameplayAttributeData CriticalHitChance;
	ATTRIBUTE_ACCESSORS(UElectricCastleAttributeSet, CriticalHitChance);
	UFUNCTION()
	FORCEINLINE void OnRep_CriticalHitChance(const FGameplayAttributeData& OldValue) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UElectricCastleAttributeSet, CriticalHitChance, OldValue);
	}

	/**
	 * Vital Attributes
	 */
	// Health
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Vital Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UElectricCastleAttributeSet, Health);
	UFUNCTION()
	FORCEINLINE void OnRep_Health(const FGameplayAttributeData& OldHealth) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UElectricCastleAttributeSet, Health, OldHealth);
	}

	// Mana
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Vital Attributes")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UElectricCastleAttributeSet, Mana);
	UFUNCTION()
	FORCEINLINE void OnRep_Mana(const FGameplayAttributeData& OldMana) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UElectricCastleAttributeSet, Mana, OldMana);
	}

	/**
	 *Meta Attributes
	 */
	UPROPERTY(BlueprintReadOnly, Category="Meta Attributes")
	FGameplayAttributeData Meta_IncomingDamage;
	ATTRIBUTE_ACCESSORS(UElectricCastleAttributeSet, Meta_IncomingDamage);
	UPROPERTY(BlueprintReadOnly, Category="Meta Attributes")
	FGameplayAttributeData Meta_IncomingXP;
	ATTRIBUTE_ACCESSORS(UElectricCastleAttributeSet, Meta_IncomingXP);
	UPROPERTY(BlueprintReadOnly, Category="Meta Attributes")
	FGameplayAttributeData Meta_IncomingRefresh;
	ATTRIBUTE_ACCESSORS(UElectricCastleAttributeSet, Meta_IncomingRefresh);

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> InitializeVitalAttributes;

private:
	void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const;

	void HandleIncomingDamage(const FEffectProperties& Props);
	void HandleIncomingXP(const FEffectProperties& Props);
	void HandleIncomingRefresh(const FEffectProperties& Props);
	void HandleDebuff(const FEffectProperties& Props);
	void HandleOutgoingDamage(const FEffectProperties& Props, float IncomingDamage);
	void ShowDamageText(
		const FEffectProperties& Props,
		const float& IncomingDamage
	) const;
	void SendXPEvent(const FEffectProperties& Props) const;
	void SendReplenishVitalsEvent(AActor* AvatarActor);
};
