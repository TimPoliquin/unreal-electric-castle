// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 * AuraGameplayTags is a singleton containing native Gameplay tags
 */
struct FElectricCastleGameplayTags
{
	static FElectricCastleGameplayTags& Get()
	{
		if (!Instance.bInitialized)
		{
			Instance.InitializeNativeGameplayTags();
			Instance.bInitialized = true;
		}
		return Instance;
	}

	static void InitializeNativeGameplayTags();
	static bool IsLeftMouseButton(const FGameplayTag&);

	bool bInitialized = false;

	/** Primary attributes **/
	FGameplayTag Attributes_Primary_Strength;
	FGameplayTag Attributes_Primary_Intelligence;
	FGameplayTag Attributes_Primary_Resilience;
	FGameplayTag Attributes_Primary_Vigor;
	/** Secondary Attributes **/
	FGameplayTag Attributes_Secondary_Armor;
	FGameplayTag Attributes_Secondary_ArmorPenetration;
	FGameplayTag Attributes_Secondary_BlockChance;
	FGameplayTag Attributes_Secondary_CriticalHitChance;
	FGameplayTag Attributes_Secondary_CriticalHitDamage;
	FGameplayTag Attributes_Secondary_CriticalHitResistance;
	FGameplayTag Attributes_Secondary_HealthRegeneration;
	FGameplayTag Attributes_Secondary_ManaRegeneration;
	FGameplayTag Attributes_Secondary_MaxHealth;
	FGameplayTag Attributes_Secondary_MaxMana;
	/** Vital Attributes **/
	FGameplayTag Attributes_Vital_Health;
	FGameplayTag Attributes_Vital_Mana;
	/** Meta Attributes **/
	FGameplayTag Attributes_Meta_IncomingDamage;
	FGameplayTag Attributes_Meta_IncomingXP;
	FGameplayTag Attributes_Meta_IncomingRefresh;
	/** Passive Attributes **/
	FGameplayTag Attributes_Passive_Protection;
	/** Progression Attributes **/
	FGameplayTag Attributes_Progression_Level;
	FGameplayTag Attributes_Progression_XP;
	FGameplayTag Attributes_Progression_SpellPoints;
	FGameplayTag Attributes_Progression_AttributePoints;

	/** Input Tags **/
	FGameplayTag InputTag;
	FGameplayTag InputTag_LeftMouseButton;
	FGameplayTag InputTag_AttackTarget;
	FGameplayTag InputTag_RightMouseButton;
	FGameplayTag InputTag_Action1;
	FGameplayTag InputTag_Action2;
	FGameplayTag InputTag_Action3;
	FGameplayTag InputTag_Action4;
	FGameplayTag InputTag_Passive1;
	FGameplayTag InputTag_Passive2;
	FGameplayTag InputTag_Interact;
	FGameplayTag InputTag_Fishing_Reel;
	FGameplayTag InputTag_Cancel;

	/** Effect Tags **/
	FGameplayTag Effect_HitReact;
	FGameplayTag Effect_HitReact_Default;
	FGameplayTag Effect_HitReact_Shock;
	FGameplayTag Effect_Magnitude;

	/** Damage Tags **/
	FGameplayTag Damage;
	FGameplayTag Damage_Arcane;
	FGameplayTag Damage_Fire;
	FGameplayTag Damage_Lightning;
	FGameplayTag Damage_Physical;

	/** Resistance Tags **/
	FGameplayTag Attributes_Resistance_Arcane;
	FGameplayTag Attributes_Resistance_Fire;
	FGameplayTag Attributes_Resistance_Lightning;
	FGameplayTag Attributes_Resistance_Physical;

	/** Debuff Tags **/
	FGameplayTag Debuff;
	FGameplayTag Debuff_Type;
	FGameplayTag Debuff_Type_Burn;
	FGameplayTag Debuff_Type_Shock;
	FGameplayTag Debuff_Type_Curse;
	FGameplayTag Debuff_Type_Bleed;
	FGameplayTag Debuff_Stat;
	FGameplayTag Debuff_Stat_Chance;
	FGameplayTag Debuff_Stat_Damage;
	FGameplayTag Debuff_Stat_Frequency;
	FGameplayTag Debuff_Stat_Duration;
	FGameplayTag Debuff_Block_Regen_Health;
	FGameplayTag Debuff_Block_Regen_Mana;

	FGameplayTag Effect_Passive_HaloOfProtection;


	/** Abilities **/
	FGameplayTag Abilities;
	FGameplayTag Abilities_Attack;
	FGameplayTag Abilities_HitReact;
	FGameplayTag Abilities_Summon;

	FGameplayTag Abilities_Offensive;
	FGameplayTag Abilities_Offensive_Fire_FireBolt;
	FGameplayTag Abilities_Offensive_Fire_FireBlast;
	FGameplayTag Abilities_Offensive_Lightning_Electrocute;
	FGameplayTag Abilities_Offensive_Arcane_ArcaneShards;
	FGameplayTag Abilities_Passive;
	FGameplayTag Abilities_Passive_ListenForEvent;
	FGameplayTag Abilities_Passive_HaloOfProtection;
	FGameplayTag Abilities_Passive_LifeSiphon;
	FGameplayTag Abilities_Passive_ManaSiphon;
	FGameplayTag Abilities_Other_Interact;
	FGameplayTag Abilities_Other_Cancel;
	FGameplayTag Abilities_Fishing;
	FGameplayTag Abilities_Fishing_Reel;
	FGameplayTag Abilities_Status;
	FGameplayTag Abilities_Status_Locked;
	FGameplayTag Abilities_Status_Eligible;
	FGameplayTag Abilities_Status_Unlocked;
	FGameplayTag Abilities_Status_Equipped;
	FGameplayTag Abilities_Type;
	FGameplayTag Abilities_Type_Offensive;
	FGameplayTag Abilities_Type_Passive;
	FGameplayTag Abilities_Type_None;

	/** Event Tags **/
	FGameplayTag Event_Montage_Electrocute;

	/** Cooldown **/
	FGameplayTag Cooldown_Fire_FireBolt;
	FGameplayTag Cooldown_Fire_FireBlast;
	FGameplayTag Cooldown_Lightning_Electrocute;
	FGameplayTag Cooldown_Arcane_ArcaneShards;

	/** Combat Socket Tags */
	FGameplayTag Combat_Socket_Weapon;
	FGameplayTag Combat_Socket_LeftHand;
	FGameplayTag Combat_Socket_RightHand;
	FGameplayTag Combat_Socket_Tail;

	/** Player Tags **/
	FGameplayTag Player_Block_CursorTrace;
	FGameplayTag Player_Block_Movement;
	FGameplayTag Player_Block_Ability_Offensive;
	FGameplayTag Player_Block_Interaction;
	FGameplayTag Player_State_InteractionAvailable;
	FGameplayTag Player_Equipped_Tool;
	FGameplayTag Player_Equipped_Tool_FishingRod;
	FGameplayTag Player_Equipped_Weapon;
	FGameplayTag Player_POI_Fishing;
	FGameplayTag Player_POI_Fishing_CanReel;
	FGameplayTag Player_HUD_Hide;

	/* Item Tags */
	FGameplayTag Item;
	FGameplayTag Item_Type;
	FGameplayTag Item_Type_Consumable;
	FGameplayTag Item_Type_Treasure;
	FGameplayTag Item_Type_Equipment;
	FGameplayTag Item_Type_Fish;
	FGameplayTag Item_Type_Instant;
	FGameplayTag Item_Type_Key;
	FGameplayTag Item_Type_None;

	/* Equipment Tags */
	FGameplayTag Item_Type_Equipment_Staff;
	FGameplayTag Item_Type_Equipment_FishingRod;

	/* Equipment Tags */
	FGameplayTag Equipment;
	FGameplayTag Equipment_Type;
	FGameplayTag Equipment_Type_None;
	FGameplayTag Equipment_Type_Tool;
	FGameplayTag Equipment_Type_Weapon;
	FGameplayTag Equipment_Slot;
	FGameplayTag Equipment_Slot_None;
	FGameplayTag Equipment_Slot_Tool;
	FGameplayTag Equipment_Slot_Weapon;

	/* Fishing Tags */
	FGameplayTag Item_Type_Fish_None;
	FGameplayTag Fish;
	FGameplayTag Fish_Rarity;
	FGameplayTag Fish_Tag;

	/** Message Tags **/
	FGameplayTag Message_Default_Item_Pickup;
	FGameplayTag Message_Default_Item_Use;


	/**
	 * Montage Attack Tags 
	 */
	uint8 MontageAttackNum = 4;
	TArray<FGameplayTag> Montage_Attack_Tags;

	FORCEINLINE const TArray<FGameplayTag>& GetDamageTypes() const
	{
		return DamageTypes;
	}

	FORCEINLINE FGameplayTag GetDamageTypeResistanceTag(const FGameplayTag& DamageType) const
	{
		return DamageTypesToResistances[DamageType];
	}

	FORCEINLINE FGameplayTag GetDamageTypeDebuffTag(const FGameplayTag& DamageType) const
	{
		return DamageTypesToDebuffs[DamageType];
	}

	FORCEINLINE FGameplayTag GetDebuffTypeDamageTypeTag(const FGameplayTag& DebuffType) const
	{
		return DebuffTypeToDamageType[DebuffType];
	}

private:
	static FElectricCastleGameplayTags Instance;
	TArray<FGameplayTag> DamageTypes;
	TMap<FGameplayTag, FGameplayTag> DamageTypesToResistances;
	TMap<FGameplayTag, FGameplayTag> DamageTypesToDebuffs;
	TMap<FGameplayTag, FGameplayTag> DebuffTypeToDamageType;
};
