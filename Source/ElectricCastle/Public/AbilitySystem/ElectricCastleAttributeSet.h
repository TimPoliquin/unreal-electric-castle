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
	// Strength
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Strength, Category = "Primary Attributes")
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(UElectricCastleAttributeSet, Strength);
	UFUNCTION()
	FORCEINLINE void OnRep_Strength(const FGameplayAttributeData& OldStrength) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UElectricCastleAttributeSet, Strength, OldStrength);
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

	// Resilience
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Resilience, Category = "Primary Attributes")
	FGameplayAttributeData Resilience;
	ATTRIBUTE_ACCESSORS(UElectricCastleAttributeSet, Resilience);
	UFUNCTION()
	FORCEINLINE void OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UElectricCastleAttributeSet, Resilience, OldResilience);
	}

	// Vigor
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Vigor, Category = "Primary Attributes")
	FGameplayAttributeData Vigor;
	ATTRIBUTE_ACCESSORS(UElectricCastleAttributeSet, Vigor);
	UFUNCTION()
	FORCEINLINE void OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UElectricCastleAttributeSet, Vigor, OldVigor);
	}

	/**
	 * Secondary Attributes
	 */
	// Armor
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Armor, Category = "Secondary Attributes")
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UElectricCastleAttributeSet, Armor);
	UFUNCTION()
	FORCEINLINE void OnRep_Armor(const FGameplayAttributeData& OldArmor) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UElectricCastleAttributeSet, Armor, OldArmor);
	}

	// Armor Penetration
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ArmorPenetration, Category = "Secondary Attributes")
	FGameplayAttributeData ArmorPenetration;
	ATTRIBUTE_ACCESSORS(UElectricCastleAttributeSet, ArmorPenetration);
	UFUNCTION()
	FORCEINLINE void OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UElectricCastleAttributeSet, ArmorPenetration, OldArmorPenetration);
	}

	// BlockChance
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BlockChance, Category = "Secondary Attributes")
	FGameplayAttributeData BlockChance;
	ATTRIBUTE_ACCESSORS(UElectricCastleAttributeSet, BlockChance);
	UFUNCTION()
	FORCEINLINE void OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UElectricCastleAttributeSet, BlockChance, OldBlockChance);
	}

	// CriticalHitChance
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalHitChance, Category = "Secondary Attributes")
	FGameplayAttributeData CriticalHitChance;
	ATTRIBUTE_ACCESSORS(UElectricCastleAttributeSet, CriticalHitChance);
	UFUNCTION()
	FORCEINLINE void OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UElectricCastleAttributeSet, CriticalHitChance, OldCriticalHitChance);
	}

	// CriticalHitDamage
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalHitDamage, Category = "Secondary Attributes")
	FGameplayAttributeData CriticalHitDamage;
	ATTRIBUTE_ACCESSORS(UElectricCastleAttributeSet, CriticalHitDamage);
	UFUNCTION()
	FORCEINLINE void OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UElectricCastleAttributeSet, CriticalHitDamage, OldCriticalHitDamage);
	}

	// CriticalHitResistance
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalHitResistance, Category = "Secondary Attributes")
	FGameplayAttributeData CriticalHitResistance;
	ATTRIBUTE_ACCESSORS(UElectricCastleAttributeSet, CriticalHitResistance);
	UFUNCTION()
	FORCEINLINE void OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UElectricCastleAttributeSet, CriticalHitResistance, OldCriticalHitResistance);
	}

	// HealthRegeneration
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealthRegeneration, Category = "Secondary Attributes")
	FGameplayAttributeData HealthRegeneration;
	ATTRIBUTE_ACCESSORS(UElectricCastleAttributeSet, HealthRegeneration);
	UFUNCTION()
	FORCEINLINE void OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UElectricCastleAttributeSet, HealthRegeneration, OldHealthRegeneration);
	}

	// ManaRegeneration
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ManaRegeneration, Category = "Secondary Attributes")
	FGameplayAttributeData ManaRegeneration;
	ATTRIBUTE_ACCESSORS(UElectricCastleAttributeSet, ManaRegeneration);
	UFUNCTION()
	FORCEINLINE void OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UElectricCastleAttributeSet, ManaRegeneration, OldManaRegeneration);
	}

	// Max Health
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Secondary Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UElectricCastleAttributeSet, MaxHealth);
	UFUNCTION()
	FORCEINLINE void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UElectricCastleAttributeSet, MaxHealth, OldMaxHealth);
	}

	// Max Mana
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Secondary Attributes")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UElectricCastleAttributeSet, MaxMana);
	UFUNCTION()
	FORCEINLINE void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UElectricCastleAttributeSet, MaxMana, OldMaxMana);
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

	/** Resistances **/

	// Resistance - Arcane
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ResistanceArcane, Category = "Resistance Attributes")
	FGameplayAttributeData Resistance_Arcane;
	ATTRIBUTE_ACCESSORS(UElectricCastleAttributeSet, Resistance_Arcane);
	UFUNCTION()
	FORCEINLINE void OnRep_ResistanceArcane(const FGameplayAttributeData& OldValue) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UElectricCastleAttributeSet, Resistance_Arcane, OldValue);
	}

	// Resistance - Fire
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ResistanceFire, Category = "Resistance Attributes")
	FGameplayAttributeData Resistance_Fire;
	ATTRIBUTE_ACCESSORS(UElectricCastleAttributeSet, Resistance_Fire);
	UFUNCTION()
	FORCEINLINE void OnRep_ResistanceFire(const FGameplayAttributeData& OldValue) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UElectricCastleAttributeSet, Resistance_Fire, OldValue);
	}

	// Resistance - Lightning
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ResistanceLightning, Category = "Resistance Attributes")
	FGameplayAttributeData Resistance_Lightning;
	ATTRIBUTE_ACCESSORS(UElectricCastleAttributeSet, Resistance_Lightning);
	UFUNCTION()
	FORCEINLINE void OnRep_ResistanceLightning(const FGameplayAttributeData& OldValue) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UElectricCastleAttributeSet, Resistance_Lightning, OldValue);
	}

	// Resistance - Physical
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ResistancePhysical, Category = "Resistance Attributes")
	FGameplayAttributeData Resistance_Physical;
	ATTRIBUTE_ACCESSORS(UElectricCastleAttributeSet, Resistance_Physical);
	UFUNCTION()
	FORCEINLINE void OnRep_ResistancePhysical(const FGameplayAttributeData& OldValue) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UElectricCastleAttributeSet, Resistance_Physical, OldValue);
	}

	/**	 Passive Attributes  */
	/**
	 * Protection afforded by passive abilities.
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_PassiveProtection, Category="Passive Attributes")
	FGameplayAttributeData Passive_Protection;
	ATTRIBUTE_ACCESSORS(UElectricCastleAttributeSet, Passive_Protection);
	UFUNCTION()
	FORCEINLINE void OnRep_PassiveProtection(const FGameplayAttributeData& OldValue) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UElectricCastleAttributeSet, Passive_Protection, OldValue);
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
