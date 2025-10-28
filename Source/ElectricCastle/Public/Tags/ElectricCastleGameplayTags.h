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
	FGameplayTag Attributes_Primary_MaxHealth;
	FGameplayTag Attributes_Primary_MaxMana;
	FGameplayTag Attributes_Primary_Strength;
	FGameplayTag Attributes_Primary_Agility;
	FGameplayTag Attributes_Primary_Intelligence;
	FGameplayTag Attributes_Primary_Constitution;
	FGameplayTag Attributes_Primary_Wisdom;
	/** Secondary Attributes **/
	FGameplayTag Attributes_Secondary_AttackPower;
	FGameplayTag Attributes_Secondary_MagicPower;
	FGameplayTag Attributes_Secondary_Defense;
	FGameplayTag Attributes_Secondary_MagicDefense;
	FGameplayTag Attributes_Secondary_HitChance;
	FGameplayTag Attributes_Secondary_EvadeChance;
	FGameplayTag Attributes_Secondary_CriticalHitChance;
	/** Vital Attributes **/
	FGameplayTag Attributes_Vital_Health;
	FGameplayTag Attributes_Vital_Mana;
	/** Meta Attributes **/
	FGameplayTag Attributes_Meta_IncomingDamage;
	FGameplayTag Attributes_Meta_IncomingXP;
	FGameplayTag Attributes_Meta_IncomingRefresh;
	/** Progression Attributes **/
	FGameplayTag Attributes_Progression_Level;
	FGameplayTag Attributes_Progression_XP;
	FGameplayTag Attributes_Progression_SpellPoints;

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
	FGameplayTag Effect_Damage;
	FGameplayTag Effect_Damage_Magic;
	FGameplayTag Effect_Damage_Magic_Arcane;
	FGameplayTag Effect_Damage_Magic_Dark;
	FGameplayTag Effect_Damage_Magic_Fire;
	FGameplayTag Effect_Damage_Magic_Lightning;
	FGameplayTag Effect_Damage_Magic_Psych;
	FGameplayTag Effect_Damage_Magic_Spirit;
	FGameplayTag Effect_Damage_Magic_Sun;
	FGameplayTag Effect_Damage_Magic_Tech;
	FGameplayTag Effect_Damage_Physical;

	FGameplayTag Effect_HitReact;
	FGameplayTag Effect_HitReact_Default;
	FGameplayTag Effect_HitReact_Shock;
	FGameplayTag Effect_Magnitude;

	FGameplayTag Effect_Resist;
	FGameplayTag Effect_Resist_Magic;
	FGameplayTag Effect_Resist_Magic_Arcane;
	FGameplayTag Effect_Resist_Magic_Dark;
	FGameplayTag Effect_Resist_Magic_Fire;
	FGameplayTag Effect_Resist_Magic_Lightning;
	FGameplayTag Effect_Resist_Magic_Psych;
	FGameplayTag Effect_Resist_Magic_Spirit;
	FGameplayTag Effect_Resist_Magic_Sun;
	FGameplayTag Effect_Resist_Magic_Tech;
	FGameplayTag Effect_Resist_Physical;

	/** Debuff Tags **/
	FGameplayTag Effect_Debuff;
	FGameplayTag Effect_Debuff_Type;
	FGameplayTag Effect_Debuff_Type_Burn;
	FGameplayTag Effect_Debuff_Type_Shock;
	FGameplayTag Effect_Debuff_Type_Curse;
	FGameplayTag Effect_Debuff_Type_Bleed;
	FGameplayTag Effect_Debuff_Stat;
	FGameplayTag Effect_Debuff_Level;
	FGameplayTag Effect_Debuff_Stat_Chance;
	FGameplayTag Effect_Debuff_Stat_Damage;
	FGameplayTag Effect_Debuff_Stat_Frequency;
	FGameplayTag Effect_Debuff_Stat_Duration;

	/** Abilities **/
	FGameplayTag Abilities;
	FGameplayTag Abilities_Attack;
	FGameplayTag Abilities_HitReact;
	FGameplayTag Abilities_Summon;
	FGameplayTag Abilities_Form;
	FGameplayTag Abilities_Form_Barbarian;
	FGameplayTag Abilities_Form_Egyptian;
	FGameplayTag Abilities_Form_Futureman;
	FGameplayTag Abilities_Form_Highlander;
	FGameplayTag Abilities_Form_Hippie;
	FGameplayTag Abilities_Form_Knight;
	FGameplayTag Abilities_Form_Native;
	FGameplayTag Abilities_Form_Roman;

	FGameplayTag Abilities_Barbarian;
	FGameplayTag Abilities_Egyptian;
	FGameplayTag Abilities_Futureman;
	FGameplayTag Abilities_Highlander;
	FGameplayTag Abilities_Hippie;
	FGameplayTag Abilities_Knight;
	FGameplayTag Abilities_Native;
	FGameplayTag Abilities_Roman;
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
	FGameplayTag Cooldown_Form;
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
	FGameplayTag Player_Block_ChangeForm;
	FGameplayTag Player_Form;
	FGameplayTag Player_Form_Barbarian;
	FGameplayTag Player_Form_Egyptian;
	FGameplayTag Player_Form_Futureman;
	FGameplayTag Player_Form_Highlander;
	FGameplayTag Player_Form_Hippie;
	FGameplayTag Player_Form_Knight;
	FGameplayTag Player_Form_Native;
	FGameplayTag Player_Form_Roman;
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

	TArray<FGameplayTag> GetDamageTypes() const;
	TArray<FGameplayTag> GetDebuffTypes() const;
	FGameplayTag GetDamageTypeResistanceTag(const FGameplayTag& DamageTypeTag) const;

private:
	static FElectricCastleGameplayTags Instance;

	TArray<FGameplayTag> DamageTypes;
	TArray<FGameplayTag> DebuffTypes;
	TMap<FGameplayTag, FGameplayTag> DamageTypeToResistanceType;
};
