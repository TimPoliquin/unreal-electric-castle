// Copyright Alien Shores


#include "Tags/ElectricCastleGameplayTags.h"
#include "GameplayTagsManager.h"

FElectricCastleGameplayTags FElectricCastleGameplayTags::Instance;

void FElectricCastleGameplayTags::InitializeNativeGameplayTags()
{
	if (Instance.bInitialized)
	{
		return;
	}
	UGameplayTagsManager& TagManager = UGameplayTagsManager::Get();
	Instance.Attributes_Primary_Strength = TagManager.AddNativeGameplayTag(
		FName("Attributes.Primary.Strength"),
		FString("Increases physical damage")
	);
	Instance.Attributes_Primary_Intelligence = TagManager.AddNativeGameplayTag(
		FName("Attributes.Primary.Intelligence"),
		FString("Increases magical damage")
	);
	Instance.Attributes_Primary_Resilience = TagManager.AddNativeGameplayTag(
		FName("Attributes.Primary.Resilience"),
		FString("Increases armor and armor penetration")
	);
	Instance.Attributes_Primary_Vigor = TagManager.AddNativeGameplayTag(
		FName("Attributes.Primary.Vigor"),
		FString("Increases max health and health regen")
	);

	// Secondary Attributes
	Instance.Attributes_Secondary_Armor = TagManager.AddNativeGameplayTag(
		FName("Attributes.Secondary.Armor"),
		FString("Reduces damage taken; improves block chance")
	);
	Instance.Attributes_Secondary_ArmorPenetration = TagManager.AddNativeGameplayTag(
		FName("Attributes.Secondary.ArmorPenetration"),
		FString("")
	);
	Instance.Attributes_Secondary_BlockChance = TagManager.AddNativeGameplayTag(
		FName("Attributes.Secondary.BlockChance"),
		FString("Reduces damage taken, improves Block Chance")
	);
	Instance.Attributes_Secondary_CriticalHitChance = TagManager.AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitChance"),
		FString("")
	);
	Instance.Attributes_Secondary_CriticalHitDamage = TagManager.AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitDamage"),
		FString("")
	);
	Instance.Attributes_Secondary_CriticalHitResistance = TagManager.AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitResistance"),
		FString("")
	);
	Instance.Attributes_Secondary_HealthRegeneration = TagManager.AddNativeGameplayTag(
		FName("Attributes.Secondary.HealthRegeneration"),
		FString("")
	);
	Instance.Attributes_Secondary_ManaRegeneration = TagManager.AddNativeGameplayTag(
		FName("Attributes.Secondary.ManaRegeneration"),
		FString("")
	);
	Instance.Attributes_Secondary_MaxHealth = TagManager.AddNativeGameplayTag(
		FName("Attributes.Secondary.MaxHealth"),
		FString("Maximum health")
	);
	Instance.Attributes_Secondary_MaxMana = TagManager.AddNativeGameplayTag(
		FName("Attributes.Secondary.MaxMana"),
		FString("Maximum mana")
	);
	Instance.Attributes_Vital_Health = TagManager.AddNativeGameplayTag(
		FName("Attributes.Vital.Health"),
		FString("Current Health")
	);
	Instance.Attributes_Vital_Mana = TagManager.AddNativeGameplayTag(
		FName("Attributes.Vital.Mana"),
		FString("Current Mana")
	);
	// Meta Attributes
	Instance.Attributes_Meta_IncomingDamage = TagManager.AddNativeGameplayTag(
		FName("Attributes.Meta.IncomingDamage"),
		FString("Incoming damage")
	);
	Instance.Attributes_Meta_IncomingXP = TagManager.AddNativeGameplayTag(
		FName("Attributes.Meta.IncomingXP"),
		FString("Incoming XP")
	);
	Instance.Attributes_Meta_IncomingRefresh = TagManager.AddNativeGameplayTag(
		FName("Attributes.Meta.IncomingRefresh"),
		FString("Incoming Refresh after a Level Up")
	);
	// Passive Attributes
	Instance.Attributes_Passive_Protection = TagManager.AddNativeGameplayTag(
		FName("Attributes.Passive.Protection"),
		FString("Protection afforded by passive abilities")
	);
	// Progression attributes
	Instance.Attributes_Progression_Level = TagManager.AddNativeGameplayTag(
		FName("Attributes.Progression.Level"),
		FString("The tag representation the character's progression level")
	);
	Instance.Attributes_Progression_XP = TagManager.AddNativeGameplayTag(
		FName("Attributes.Progression.XP"),
		FString("The tag representation the character's XP")
	);
	Instance.Attributes_Progression_SpellPoints = TagManager.AddNativeGameplayTag(
		FName("Attributes.Progression.SpellPoints"),
		FString("The tag representation the character's spell points")
	);
	Instance.Attributes_Progression_AttributePoints = TagManager.AddNativeGameplayTag(
		FName("Attributes.Progression.AttributePoints"),
		FString("The tag representation the character's attribute points")
	);

	// Input Tags
	Instance.InputTag = TagManager.AddNativeGameplayTag(
		FName("InputTag"),
		FString("Input tag root")
	);
	Instance.InputTag_LeftMouseButton = TagManager.AddNativeGameplayTag(
		FName("InputTag.LeftMouseButton"),
		FString("Input tag for the Left Mouse Button")
	);
	Instance.InputTag_AttackTarget = TagManager.AddNativeGameplayTag(
		FName("InputTag.AttackTarget"),
		FString("Input tag for the Attack Target (Shift + LMB)")
	);
	Instance.InputTag_RightMouseButton = TagManager.AddNativeGameplayTag(
		FName("InputTag.RightMouseButton"),
		FString("Input tag for the Right Mouse Button")
	);
	Instance.InputTag_Action1 = TagManager.AddNativeGameplayTag(
		FName("InputTag.Action1"),
		FString("Input tag for Action 1")
	);
	Instance.InputTag_Action2 = TagManager.AddNativeGameplayTag(
		FName("InputTag.Action2"),
		FString("Input tag for Action 2")
	);
	Instance.InputTag_Action3 = TagManager.AddNativeGameplayTag(
		FName("InputTag.Action3"),
		FString("Input tag for Action 3")
	);
	Instance.InputTag_Action4 = TagManager.AddNativeGameplayTag(
		FName("InputTag.Action4"),
		FString("Input tag for Action 4")
	);
	Instance.InputTag_Passive1 = TagManager.AddNativeGameplayTag(
		FName("InputTag.Passive1"),
		FString("Input tag for Passive Ability 1")
	);
	Instance.InputTag_Passive2 = TagManager.AddNativeGameplayTag(
		FName("InputTag.Passive2"),
		FString("Input tag for Passive Ability 2")
	);
	Instance.InputTag_Interact = TagManager.AddNativeGameplayTag(
		FName("InputTag.Interact"),
		FString("Input tag for player interacting with an object")
	);
	Instance.InputTag_Fishing_Reel = TagManager.AddNativeGameplayTag(
		FName("InputTag.Fishing.Reel"),
		FString("Input tag for reeling in the fishing line during the fishing minigame")
	);
	Instance.InputTag_Cancel = TagManager.AddNativeGameplayTag(
		FName("InputTag.Cancel"),
		FString("Input tag for canceling player interaction and abilities")
	);

	// Effect Tags
	Instance.Effect_HitReact = TagManager.AddNativeGameplayTag(
		FName("Effect.HitReact"),
		FString("Tag applied when character is reacting to a hit")
	);
	Instance.Effect_HitReact_Default = TagManager.AddNativeGameplayTag(
		FName("Effect.HitReact.Default"),
		FString("Tag applied when character is reacting to a hit")
	);
	Instance.Effect_HitReact_Shock = TagManager.AddNativeGameplayTag(
		FName("Effect.HitReact.Shock"),
		FString("Tag applied when character is reacting to a hit with shock")
	);
	Instance.Effect_Magnitude = TagManager.AddNativeGameplayTag(
		FName("Effect.Magnitude"),
		FString("Generic tag for specifying magnitude of effect across multiple effects")
	);

	// Damage Tags
	Instance.Damage = TagManager.AddNativeGameplayTag(
		FName("Damage"),
		FString("Damage")
	);
	Instance.Damage_Arcane = TagManager.AddNativeGameplayTag(
		FName("Damage.Arcane"),
		FString("Arcane Damage")
	);
	Instance.Damage_Fire = TagManager.AddNativeGameplayTag(
		FName("Damage.Fire"),
		FString("Fire Damage")
	);
	Instance.Damage_Lightning = TagManager.AddNativeGameplayTag(
		FName("Damage.Lightning"),
		FString("Lightning Damage")
	);
	Instance.Damage_Physical = TagManager.AddNativeGameplayTag(
		FName("Damage.Physical"),
		FString("Physical Damage")
	);
	Instance.DamageTypes.Add(Instance.Damage_Arcane);
	Instance.DamageTypes.Add(Instance.Damage_Fire);
	Instance.DamageTypes.Add(Instance.Damage_Lightning);
	Instance.DamageTypes.Add(Instance.Damage_Physical);

	// Resistances
	Instance.Attributes_Resistance_Arcane = TagManager.AddNativeGameplayTag(
		FName("Attributes.Resistance.Arcane"),
		FString("Arcane Damage Resistance")
	);
	Instance.Attributes_Resistance_Fire = TagManager.AddNativeGameplayTag(
		FName("Attributes.Resistance.Fire"),
		FString("Fire Damage Resistance")
	);
	Instance.Attributes_Resistance_Lightning = TagManager.AddNativeGameplayTag(
		FName("Attributes.Resistance.Lightning"),
		FString("Lightning Damage Resistance")
	);
	Instance.Attributes_Resistance_Physical = TagManager.AddNativeGameplayTag(
		FName("Attributes.Resistance.Physical"),
		FString("Physical Damage Resistance")
	);
	// Map damage type to resistance attribute
	Instance.DamageTypesToResistances.Add(Instance.Damage_Arcane, Instance.Attributes_Resistance_Arcane);
	Instance.DamageTypesToResistances.Add(Instance.Damage_Fire, Instance.Attributes_Resistance_Fire);
	Instance.DamageTypesToResistances.Add(Instance.Damage_Lightning, Instance.Attributes_Resistance_Lightning);
	Instance.DamageTypesToResistances.Add(Instance.Damage_Physical, Instance.Attributes_Resistance_Physical);

	Instance.DamageTypesToResistances.GenerateKeyArray(Instance.DamageTypes);

	Instance.Debuff = TagManager.AddNativeGameplayTag(
		FName("Debuff"),
		FString("Root tag for all debuff related tags")
	);
	Instance.Debuff_Type = TagManager.AddNativeGameplayTag(
		FName("Debuff.Type"),
		FString("Root tag for all types of debuffs")
	);
	Instance.Debuff_Type_Burn = TagManager.AddNativeGameplayTag(
		FName("Debuff.Type.Burn"),
		FString("Burns target for damage for a duration")
	);
	Instance.Debuff_Type_Shock = TagManager.AddNativeGameplayTag(
		FName("Debuff.Type.Shock"),
		FString("Shocks target for damage for a duration")
	);
	Instance.Debuff_Type_Curse = TagManager.AddNativeGameplayTag(
		FName("Debuff.Type.Curse"),
		FString("Curse target for damage for a duration")
	);
	Instance.Debuff_Type_Bleed = TagManager.AddNativeGameplayTag(
		FName("Debuff.Type.Bleed"),
		FString("Bleed target for damage for a duration")
	);
	Instance.DamageTypesToDebuffs.Add(Instance.Damage_Arcane, Instance.Debuff_Type_Curse);
	Instance.DamageTypesToDebuffs.Add(Instance.Damage_Fire, Instance.Debuff_Type_Burn);
	Instance.DamageTypesToDebuffs.Add(Instance.Damage_Lightning, Instance.Debuff_Type_Shock);
	Instance.DamageTypesToDebuffs.Add(Instance.Damage_Physical, Instance.Debuff_Type_Bleed);

	Instance.DebuffTypeToDamageType.Add(Instance.Debuff_Type_Bleed, Instance.Damage_Physical);
	Instance.DebuffTypeToDamageType.Add(Instance.Debuff_Type_Burn, Instance.Damage_Fire);
	Instance.DebuffTypeToDamageType.Add(Instance.Debuff_Type_Curse, Instance.Damage_Arcane);
	Instance.DebuffTypeToDamageType.Add(Instance.Debuff_Type_Shock, Instance.Damage_Lightning);

	Instance.Debuff_Stat = TagManager.AddNativeGameplayTag(
		FName("Debuff.Stat"),
		FString("Root tag for debuff stat tags")
	);
	Instance.Debuff_Stat_Chance = TagManager.AddNativeGameplayTag(
		FName("Debuff.Stat.Chance"),
		FString("Debuff chance")
	);
	Instance.Debuff_Stat_Damage = TagManager.AddNativeGameplayTag(
		FName("Debuff.Stat.Damage"),
		FString("Debuff Damage")
	);
	Instance.Debuff_Stat_Frequency = TagManager.AddNativeGameplayTag(
		FName("Debuff.Stat.Frequency"),
		FString("Debuff Frequency")
	);
	Instance.Debuff_Stat_Duration = TagManager.AddNativeGameplayTag(
		FName("Debuff.Stat.Duration"),
		FString("Debuff Duration")
	);
	Instance.Debuff_Block_Regen_Health = TagManager.AddNativeGameplayTag(
		FName("Debuff.Block.Regen.Health"),
		FString("Block health regen")
	);
	Instance.Debuff_Block_Regen_Mana = TagManager.AddNativeGameplayTag(
		FName("Debuff.Block.Regen.Mana"),
		FString("Block mana regen")
	);
	/** Ability Tags */
	Instance.Abilities = TagManager.AddNativeGameplayTag(FName("Abilities"), FString("Abilities root tag"));
	Instance.Abilities_HitReact = TagManager.AddNativeGameplayTag(
		FName("Abilities.HitReact"),
		FString("Tag applied when character is reacting to a hit")
	);
	Instance.Abilities_Attack = TagManager.AddNativeGameplayTag(
		FName("Abilities.HitReact"),
		FString("Attack Abilities parent tag")
	);
	Instance.Abilities_Summon = TagManager.AddNativeGameplayTag(
		FName("Abilities.Summon"),
		FString("Summon Abilities parent tag")
	);
	Instance.Abilities_Offensive = TagManager.AddNativeGameplayTag(
		FName("Abilities.Offensive"),
		FString("Root tag for offensive abilities")
	);
	Instance.Abilities_Offensive_Fire_FireBolt = TagManager.AddNativeGameplayTag(
		FName("Abilities.Offensive.Fire.FireBolt"),
		FString("FireBolt ability tag")
	);
	Instance.Abilities_Offensive_Fire_FireBlast = TagManager.AddNativeGameplayTag(
		FName("Abilities.Offensive.Fire.FireBlast"),
		FString("FireBlast ability tag")
	);
	Instance.Abilities_Offensive_Lightning_Electrocute = TagManager.AddNativeGameplayTag(
		FName("Abilities.Offensive.Lightning.Electrocute"),
		FString("Electrocute ability tag")
	);
	Instance.Abilities_Offensive_Arcane_ArcaneShards = TagManager.AddNativeGameplayTag(
		FName("Abilities.Offensive.Arcane.ArcaneShards"),
		FString("ArcaneShards ability tag")
	);
	Instance.Abilities_Passive = TagManager.AddNativeGameplayTag(
		FName("Abilities.Passive"),
		FString("Passive ability tag")
	);
	Instance.Abilities_Passive_ListenForEvent = TagManager.AddNativeGameplayTag(
		FName("Abilities.Passive.ListenForEvent"),
		FString("ListenForEvent ability tag")
	);
	Instance.Abilities_Passive_HaloOfProtection = TagManager.AddNativeGameplayTag(
		FName("Abilities.Passive.HaloOfProtection"),
		FString("Passive 1 ability tag")
	);
	Instance.Abilities_Passive_LifeSiphon = TagManager.AddNativeGameplayTag(
		FName("Abilities.Passive.LifeSiphon"),
		FString("Passive 2 ability tag")
	);
	Instance.Abilities_Passive_ManaSiphon = TagManager.AddNativeGameplayTag(
		FName("Abilities.Passive.ManaSiphon"),
		FString("Passive 3 ability tag")
	);
	Instance.Abilities_Other_Interact = TagManager.AddNativeGameplayTag(
		FName("Abilities.Other.Interact"),
		FString("Interact with objects in the world")
	);
	Instance.Abilities_Other_Cancel = TagManager.AddNativeGameplayTag(
		FName("Abilities.Other.Cancel"),
		FString("Cancels active abilities and interactions")
	);
	Instance.Abilities_Fishing = TagManager.AddNativeGameplayTag(
		FName("Abilities.Fishing"),
		FString("Player's fishing ability.")
	);
	Instance.Abilities_Fishing_Reel = TagManager.AddNativeGameplayTag(
		FName("Abilities.Fishing.Reel"),
		FString("Player's fishing reel ability.")
	);
	Instance.Abilities_Status = TagManager.AddNativeGameplayTag(
		FName("Abilities.Status"),
		FString("Status Ability root")
	);
	Instance.Abilities_Status_Locked = TagManager.AddNativeGameplayTag(
		FName("Abilities.Status.Locked"),
		FString("Locked Status")
	);
	Instance.Abilities_Status_Eligible = TagManager.AddNativeGameplayTag(
		FName("Abilities.Status.Eligible"),
		FString("Eligible Status")
	);
	Instance.Abilities_Status_Unlocked = TagManager.AddNativeGameplayTag(
		FName("Abilities.Status.Unlocked"),
		FString("Unlocked Status")
	);
	Instance.Abilities_Status_Equipped = TagManager.AddNativeGameplayTag(
		FName("Abilities.Status.Equipped"),
		FString("Equipped Status")
	);
	Instance.Abilities_Type = TagManager.AddNativeGameplayTag(
		FName("Abilities.Type"),
		FString("Ability Types root")
	);
	Instance.Abilities_Type_Offensive = TagManager.AddNativeGameplayTag(
		FName("Abilities.Type.Offensive"),
		FString("Offensive Type")
	);
	Instance.Abilities_Type_Passive = TagManager.AddNativeGameplayTag(
		FName("Abilities.Type.Passive"),
		FString("Passive Type")
	);
	Instance.Abilities_Type_None = TagManager.AddNativeGameplayTag(
		FName("Abilities.Type.None"),
		FString("None Type")
	);
	/** Event Tags **/
	Instance.Event_Montage_Electrocute = TagManager.AddNativeGameplayTag(
		FName("Event.Montage.Electrocute"),
		FString("Event fired during electrocute montage")
	);
	/** Cooldown Tags **/
	Instance.Cooldown_Fire_FireBolt = TagManager.AddNativeGameplayTag(
		FName("Cooldown.Fire.FireBolt"),
		FString("FireBolt ability cooldown tag")
	);
	Instance.Cooldown_Fire_FireBlast = TagManager.AddNativeGameplayTag(
		FName("Cooldown.Fire.FireBlast"),
		FString("FireBlast ability cooldown tag")
	);
	Instance.Cooldown_Lightning_Electrocute = TagManager.AddNativeGameplayTag(
		FName("Cooldown.Lightning.Electrocute"),
		FString("Electrocute ability cooldown tag")
	);
	Instance.Cooldown_Arcane_ArcaneShards = TagManager.AddNativeGameplayTag(
		FName("Cooldown.Arcane.ArcaneShards"),
		FString("ArcaneShards ability cooldown tag")
	);

	/** Combat Socket Tags **/
	Instance.Combat_Socket_Weapon = TagManager.AddNativeGameplayTag(
		FName("Combat.Socket.Weapon"),
		FString("Weapon Socket")
	);
	Instance.Combat_Socket_RightHand = TagManager.AddNativeGameplayTag(
		FName("Combat.Socket.RightHand"),
		FString("Right Hand Socket")
	);
	Instance.Combat_Socket_LeftHand = TagManager.AddNativeGameplayTag(
		FName("Combat.Socket.LeftHand"),
		FString("Left Hand Socket")
	);
	Instance.Combat_Socket_Tail = TagManager.AddNativeGameplayTag(
		FName("Combat.Socket.Tail"),
		FString("Tail Socket")
	);

	/**
	 *Montage Attack Tags
	 */
	for (int32 i = 0; i < Instance.MontageAttackNum; i++)
	{
		Instance.Montage_Attack_Tags.Add(
			TagManager.AddNativeGameplayTag(
				FName(*FString::Printf(TEXT("Montage.Attack.%i"), i)),
				FString("Attack montage tag")
			)
		);
	}

	/** Player Tags **/
	Instance.Player_Block_CursorTrace = TagManager.AddNativeGameplayTag(
		FName("Player.Block.CursorTrace"),
		FString("Block tracing under the cursor")
	);
	Instance.Player_Block_Movement = TagManager.AddNativeGameplayTag(
		FName("Player.Block.Movement"),
		FString("Blocks player movement")
	);
	Instance.Player_Block_Interaction = TagManager.AddNativeGameplayTag(
		FName("Player.Block.Interaction"),
		FString("Blocks player's ability to interact with the environment")
	);
	Instance.Player_Block_Ability_Offensive = TagManager.AddNativeGameplayTag(
		FName("Player.Block.Ability.Offensive"),
		FString("Blocks player offensive abilities")
	);
	Instance.Player_State_InteractionAvailable = TagManager.AddNativeGameplayTag(
		FName("Player.State.InteractionAvailable"),
		FString("Interaction is available for the player")
	);
	Instance.Player_Equipped_Tool = TagManager.AddNativeGameplayTag(
		FName("Player.Equipped.Tool"),
		FString("Player has a tool equipped")
	);
	Instance.Player_Equipped_Tool = TagManager.AddNativeGameplayTag(
		FName("Player.Equipped.Tool.FishingRod"),
		FString("Player has a fishing rod tool equipped")
	);
	Instance.Player_Equipped_Weapon = TagManager.AddNativeGameplayTag(
		FName("Player.Equipped.Weapon"),
		FString("Player has a weapon equipped")
	);
	Instance.Player_POI_Fishing = TagManager.AddNativeGameplayTag(
		FName("Player.POI.Fishing"),
		FString("Player is at a fishing POI")
	);
	Instance.Player_POI_Fishing_CanReel = TagManager.AddNativeGameplayTag(
		FName("Player.POI.Fishing.CanReel"),
		FString("Player is fishing and can reel")
	);
	Instance.Player_HUD_Hide = TagManager.AddNativeGameplayTag(
		FName("Player.HUD.Hide"),
		FString("Force the player HUD to hide")
	);
	Instance.Item = TagManager.AddNativeGameplayTag(FName("Item"), FString("Item root tag"));
	Instance.Item_Type = TagManager.AddNativeGameplayTag(FName("Item.Type"), FString("Item Type root tag"));
	Instance.Item_Type_Consumable = TagManager.AddNativeGameplayTag(
		FName("Item.Type.Consumable"),
		FString("Consumable Item Type")
	);
	Instance.Item_Type_Treasure = TagManager.AddNativeGameplayTag(
		FName("Item.Type.Treasure"),
		FString("Treasure Item Type")
	);
	Instance.Item_Type_Equipment = TagManager.AddNativeGameplayTag(
		FName("Item.Type.Equipment"),
		FString("Equipment Item Type")
	);
	Instance.Item_Type_Fish = TagManager.AddNativeGameplayTag(FName("Item.Type.Fish"), FString("Fish Item Type"));
	Instance.Item_Type_Instant = TagManager.AddNativeGameplayTag(
		FName("Item.Type.Instant"),
		FString("Instant Item Type")
	);
	Instance.Item_Type_Key = TagManager.AddNativeGameplayTag(FName("Item.Type.Key"), FString("Key Item Type"));
	Instance.Item_Type_None = TagManager.AddNativeGameplayTag(FName("Item.Type.None"), FString("None Item Type"));

	Instance.Item_Type_Equipment_FishingRod = TagManager.AddNativeGameplayTag(
		FName("Item.Type.Equipment.FishingRod"),
		FString("Fishing rod - used for fishing!")
	);
	Instance.Item_Type_Equipment_Staff = TagManager.AddNativeGameplayTag(
		FName("Item.Type.Equipment.Staff"),
		FString("Staff - used for magic!")
	);

	Instance.Equipment = TagManager.AddNativeGameplayTag(FName("Equipment"), FString("Equipment root tag"));
	Instance.Equipment_Type = TagManager.AddNativeGameplayTag(
		FName("Equipment.Type"),
		FString("Equipment Type root tag")
	);
	Instance.Equipment_Type_None = TagManager.AddNativeGameplayTag(
		FName("Equipment.Type.None"),
		FString("None Equipment Type")
	);
	Instance.Equipment_Type_Tool = TagManager.AddNativeGameplayTag(
		FName("Equipment.Type.Tool"),
		FString("Tool Equipment Type")
	);
	Instance.Equipment_Type_Weapon = TagManager.AddNativeGameplayTag(
		FName("Equipment.Type.Weapon"),
		FString("Weapon Equipment Type")
	);
	Instance.Equipment_Slot = TagManager.AddNativeGameplayTag(
		FName("Equipment.Slot"),
		FString("Equipment Slot root tag")
	);
	Instance.Equipment_Slot_None = TagManager.AddNativeGameplayTag(
		FName("Equipment.Slot.None"),
		FString("None Equipment Slot")
	);
	Instance.Equipment_Slot_Tool = TagManager.AddNativeGameplayTag(
		FName("Equipment.Slot.Tool"),
		FString("Tool Equipment Slot")
	);
	Instance.Equipment_Slot_Weapon = TagManager.AddNativeGameplayTag(
		FName("Equipment.Slot.Weapon"),
		FString("Weapon Equipment Slot")
	);

	Instance.Item_Type_Fish_None = TagManager.AddNativeGameplayTag(
		FName("Item.Type.Fish.None"),
		FString("None fish type")
	);
	Instance.Fish = TagManager.AddNativeGameplayTag(FName("Fish"), FString("Fish root tag"));
	Instance.Fish_Rarity = TagManager.AddNativeGameplayTag(FName("Fish.Rarity"), FString("Fish Rarity root tag"));
	Instance.Fish_Tag = TagManager.AddNativeGameplayTag(FName("Fish.Tag"), FString("Fish Tags root tag"));
	/** Message Tags **/
	Instance.Message_Default_Item_Pickup = TagManager.AddNativeGameplayTag(FName("Message.Default.Item.Pickup"), FString("Pickup item message tag"));
	Instance.Message_Default_Item_Use = TagManager.AddNativeGameplayTag(FName("Message.Default.Item.Use"), FString("Use item message tag"));
}

bool FElectricCastleGameplayTags::IsLeftMouseButton(const FGameplayTag& InputTag)
{
	return InputTag.MatchesTagExact(Instance.InputTag_LeftMouseButton);
}
