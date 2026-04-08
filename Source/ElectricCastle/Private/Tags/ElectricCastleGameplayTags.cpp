// Copyright Alien Shores


#include "Tags/ElectricCastleGameplayTags.h"
#include "GameplayTagsManager.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"

FElectricCastleGameplayTags FElectricCastleGameplayTags::Instance;

void FElectricCastleGameplayTags::InitializeNativeGameplayTags()
{
	if (Instance.bInitialized)
	{
		return;
	}
	UGameplayTagsManager& TagManager = UGameplayTagsManager::Get();
	Instance.Attributes_Primary_MaxHealth = TagManager.AddNativeGameplayTag(
		FName("Attributes.Primary.MaxHealth"),
		FString("Maximum health")
	);
	Instance.Attributes_Primary_MaxMana = TagManager.AddNativeGameplayTag(
		FName("Attributes.Primary.MaxMana"),
		FString("Maximum mana")
	);
	Instance.Attributes_Primary_Strength = TagManager.AddNativeGameplayTag(
		FName("Attributes.Primary.Strength"),
		FString("Increases physical damage")
	);
	Instance.Attributes_Primary_Agility = TagManager.AddNativeGameplayTag(
		FName("Attributes.Primary.Agility"),
		FString("Manages speed and coordination of the character")
	);
	Instance.Attributes_Primary_Intelligence = TagManager.AddNativeGameplayTag(
		FName("Attributes.Primary.Intelligence"),
		FString("Increases magical damage")
	);
	Instance.Attributes_Primary_Constitution = TagManager.AddNativeGameplayTag(
		FName("Attributes.Primary.Constitution"),
		FString("Increases defense capabilities")
	);
	Instance.Attributes_Primary_Wisdom = TagManager.AddNativeGameplayTag(
		FName("Attributes.Primary.Wisdom"),
		FString("Increases magical defense and defensive magic capabilities")
	);

	// Secondary Attributes
	Instance.Attributes_Secondary_AttackPower = TagManager.AddNativeGameplayTag(
		FName("Attributes.Secondary.AttackPower"),
		FString("Increases outgoing physical attack damage. Strength + Weapon")
	);
	Instance.Attributes_Secondary_MagicPower = TagManager.AddNativeGameplayTag(
		FName("Attributes.Secondary.MagicPower"),
		FString("Increases outgoing magic attack damage. Intelligence + Weapon")
	);
	Instance.Attributes_Secondary_Defense = TagManager.AddNativeGameplayTag(
		FName("Attributes.Secondary.Defense"),
		FString("Reduces incoming physical attack damage. Constitution + Armor")
	);
	Instance.Attributes_Secondary_MagicDefense = TagManager.AddNativeGameplayTag(
		FName("Attributes.Secondary.MagicDefense"),
		FString("Reduces incoming magic attack damage. Wisdom + Armor")
	);
	Instance.Attributes_Secondary_HitChance = TagManager.AddNativeGameplayTag(
		FName("Attributes.Secondary.HitChance"),
		FString("Increases likelihood to deal physical damage. 75% + 1/4 Agility")
	);
	Instance.Attributes_Secondary_EvadeChance = TagManager.AddNativeGameplayTag(
		FName("Attributes.Secondary.EvadeChance"),
		FString("Increases likelihood to evade incoming physical damage. Armor + 1/4 Agility")
	);
	Instance.Attributes_Secondary_CriticalHitChance = TagManager.AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitChance"),
		FString("Increases likelihood to deal a critical hit. 5% + 1/5 Agility")
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

	// Cinematic Tags
	Instance.Cinematic = TagManager.AddNativeGameplayTag(FName("Cinematic"), FString("Root cinematic tag"));
	Instance.Cinematic_Reaction_Trigger = TagManager.AddNativeGameplayTag(FName("Cinematic.Reaction.Trigger"), FString("Root tag for cinematic reaction triggers."));
	Instance.Cinematic_Reaction_Trigger_HidePlayer = TagManager.AddNativeGameplayTag(FName("Cinematic.Reaction.Trigger.HidePlayer"), FString("When cinematic starts, hide the player"));
	Instance.Cinematic_Reaction_Trigger_HideGameOverlay = TagManager.AddNativeGameplayTag(FName("Cinematic.Reaction.Trigger.HideGameOverlay"), FString("When cinematic starts, hide the game overlay"));
	Instance.Cinematic_Reaction_Trigger_DisableStandardInput = TagManager.AddNativeGameplayTag(
		FName("Cinematic.Reaction.Trigger.DisableStandardInput"),
		FString("When cinematic starts, disable standard input")
	);
	Instance.Cinematic_Reaction_Trigger_HideNonCinematicActors = TagManager.AddNativeGameplayTag(
		FName("Cinematic.Reaction.Trigger.HideNonCinematicActors"),
		FString("When cinematic starts, hide all non-cinematic actors")
	);
	Instance.Cinematic_Type = TagManager.AddNativeGameplayTag(FName("Cinematic.Type"), FString("Root cinematic type tag"));
	Instance.Cinematic_Type_Ambient = TagManager.AddNativeGameplayTag(FName("Cinematic.Type.Ambient"), FString("Ambient cinematic; player maintains control"));
	Instance.Cinematic_Type_Cutscene = TagManager.AddNativeGameplayTag(FName("Cinematic.Type.Cutscene"), FString("Cutscene cinematic; Environment is fully controlled"));
	Instance.Cinematic_Type_Cutscene_Story = TagManager.AddNativeGameplayTag(FName("Cinematic.Type.Cutscene.Story"), FString("Cutscene cinematic; Story content."));
	Instance.Cinematic_Type_Cutscene_Intro = TagManager.AddNativeGameplayTag(FName("Cinematic.Type.Cutscene.Intro"), FString("Cutscene cinematic; Area introduction."));
	Instance.Cinematic_Type_Cutscene_Outro = TagManager.AddNativeGameplayTag(FName("Cinematic.Type.Cutscene.Outro"), FString("Cutscene cinematic; Area exit."));
	Instance.Cinematic_Type_Gameplay = TagManager.AddNativeGameplayTag(FName("Cinematic.Type.Gameplay"), FString("Gameplay cinematic"));
	Instance.Cinematic_Type_Gameplay_Dialog = TagManager.AddNativeGameplayTag(FName("Cinematic.Type.Gameplay.Dialog"), FString("Gameplay cinematic featuring dialog sequences"));
	Instance.Cinematic_Type_UI = TagManager.AddNativeGameplayTag(FName("Cinematic.Type.UI"), FString("Sequences used in menus/UI"));

	// Input Tags
	Instance.Input = TagManager.AddNativeGameplayTag(FName("Input"), FString("Input root tag"));
	Instance.Input_Common_Block = TagManager.AddNativeGameplayTag(FName("Input.Common.Block"), FString("Input tag for blocking"));
	Instance.Input_Common_Dodge = TagManager.AddNativeGameplayTag(FName("Input.Common.Dodge"), FString("Input tag for dodging"));
	Instance.Input_Common_Interact = TagManager.AddNativeGameplayTag(FName("Input.Common.Interact"), FString("Input tag for interacting with world objects"));
	Instance.Input_Ability_Aim = TagManager.AddNativeGameplayTag(FName("Input.Ability.Aim"), FString("Input tag for aiming"));
	Instance.Input_Ability_LockOn = TagManager.AddNativeGameplayTag(FName("Input.Ability.LockOn"), FString("Input tag for locking on to targets"));
	Instance.Input_Ability_Primary = TagManager.AddNativeGameplayTag(FName("Input.Ability.Primary"), FString("Input tag for primary attack"));
	Instance.Input_Ability_Secondary = TagManager.AddNativeGameplayTag(FName("Input.Ability.Secondary"), FString("Input tag for secondary attack"));
	Instance.Input_Ability_Spell_01 = TagManager.AddNativeGameplayTag(FName("Input.Ability.Spell.01"), FString("Input tag for executing spell ability 1"));
	Instance.Input_Ability_Spell_02 = TagManager.AddNativeGameplayTag(FName("Input.Ability.Spell.02"), FString("Input tag for executing spell ability 2"));
	Instance.InputTag = TagManager.AddNativeGameplayTag(FName("InputTag"));
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
	Instance.InputTag_Dialog = TagManager.AddNativeGameplayTag(FName("InputTag.Dialog"), FString("Root tag for all dialog input tags"));

	// Effect Tags
	Instance.Effect = TagManager.AddNativeGameplayTag(FName("Effect"), FString("Root effect tag"));
	Instance.Effect_Block_Damage = TagManager.AddNativeGameplayTag(FName("Effect.Block.Damage"), FString("When this tag is active, the character will not receive any damage"));
	Instance.Effect_Damage = TagManager.AddNativeGameplayTag(
		FName("Effect.Damage"),
		FString("Damage gameplay effect")
	);
	Instance.Effect_Damage_Magic = TagManager.AddNativeGameplayTag(
		FName("Effect.Damage.Magic"),
		FString("Magical damage gameplay effect")
	);
	Instance.Effect_Damage_Magic_Arcane = TagManager.AddNativeGameplayTag(
		FName("Effect.Damage.Magic.Arcane"),
		FString("Arcane damage effect")
	);
	Instance.Effect_Damage_Magic_Dark = TagManager.AddNativeGameplayTag(
		FName("Effect.Damage.Magic.Dark"),
		FString("Dark damage gameplay effect")
	);
	Instance.Effect_Damage_Magic_Fire = TagManager.AddNativeGameplayTag(
		FName("Effect.Damage.Magic.Fire"),
		FString("Fire damage gameplay effect")
	);
	Instance.Effect_Damage_Magic_Lightning = TagManager.AddNativeGameplayTag(
		FName("Effect.Damage.Magic.Lightning"),
		FString("Lightning damage gameplay effect")
	);
	Instance.Effect_Damage_Physical = TagManager.AddNativeGameplayTag(
		FName("Effect.Damage.Physical"),
		FString("Physical damage gameplay effect")
	);
	Instance.Effect_Damage_Magic_Psych = TagManager.AddNativeGameplayTag(
		FName("Effect.Damage.Magic.Psych"),
		FString("Psych damage gameplay effect")
	);
	Instance.Effect_Damage_Magic_Spirit = TagManager.AddNativeGameplayTag(
		FName("Effect.Damage.Magic.Spirit"),
		FString("Spirit damage gameplay effect")
	);
	Instance.Effect_Damage_Magic_Sun = TagManager.AddNativeGameplayTag(
		FName("Effect.Damage.Magic.Sun"),
		FString("Sun damage gameplay effect")
	);
	Instance.Effect_Damage_Magic_Tech = TagManager.AddNativeGameplayTag(
		FName("Effect.Damage.Magic.Tech"),
		FString("Tech damage gameplay effect")
	);
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
	Instance.Effect_Movement_TurnInPlace = TagManager.AddNativeGameplayTag(
		FName("Effect.Movement.TurnInPlace"),
		FString("Indicates that the player turns in place when the camera moves")
	);
	Instance.Effect_State = TagManager.AddNativeGameplayTag(
		FName("Effect.State"),
		FString("Root effect state tag")
	);
	Instance.Effect_State_Aiming = TagManager.AddNativeGameplayTag(
		FName("Effect.State.Aiming"),
		FString("Indicates that the character is aiming")
	);
	Instance.Effect_State_Blocking = TagManager.AddNativeGameplayTag(
		FName("Effect.State.Blocking"),
		FString("Indicates that the character is blocking")
	);
	Instance.Effect_State_Dodging = TagManager.AddNativeGameplayTag(FName("Effect.State.Dodging"), FString("Indicates that the character is dodging"));
	Instance.Effect_State_Jumping = TagManager.AddNativeGameplayTag(FName("Effect.State.Jumping"), FString("Indicates that the character is jumping"));
	Instance.Effect_State_LockedOn = TagManager.AddNativeGameplayTag(
		FName("Effect.State.LockedOn"),
		FString("Indicates that the character is locked on to a target")
	);
	Instance.Effect_State_Parrying = TagManager.AddNativeGameplayTag(
		FName("Effect.State.Parrying"),
		FString("Indicates that the character is within the parry window for an attack")
	);
	Instance.Effect_State_ChronoShift = TagManager.AddNativeGameplayTag(
		FName("Effect.State.ChronoShift"),
		FString("Indicates that the character is in the chrono shift state")
	);
	Instance.Effect_State_PsychedelicVision = TagManager.AddNativeGameplayTag(
		FName("Effect.State.PsychedelicVision"),
		FString("Indicates that the character is experience psychedelic visions")
	);
	Instance.Effect_Resist = TagManager.AddNativeGameplayTag(
		FName("Effect.Resist"),
		FString("Root tag for all resistance effects")
	);
	Instance.Effect_Resist_Magic = TagManager.AddNativeGameplayTag(
		FName("Effect.Resist.Magic"),
		FString("Root tag for all magical resistance effects")
	);
	Instance.Effect_Resist_Magic_Arcane = TagManager.AddNativeGameplayTag(
		FName("Effect.Resist.Magic.Arcane"),
		FString("Arcane damage resistance")
	);
	Instance.Effect_Resist_Magic_Dark = TagManager.AddNativeGameplayTag(
		FName("Effect.Resist.Magic.Dark"),
		FString("Dark damage resistance")
	);
	Instance.Effect_Resist_Magic_Fire = TagManager.AddNativeGameplayTag(
		FName("Effect.Resist.Magic.Fire"),
		FString("Fire damage resistance")
	);
	Instance.Effect_Resist_Magic_Lightning = TagManager.AddNativeGameplayTag(
		FName("Effect.Resist.Magic.Lightning"),
		FString("Lightning damage resistance")
	);
	Instance.Effect_Resist_Magic_Psych = TagManager.AddNativeGameplayTag(
		FName("Effect.Resist.Magic.Psych"),
		FString("Psych damage resistance")
	);
	Instance.Effect_Resist_Magic_Spirit = TagManager.AddNativeGameplayTag(
		FName("Effect.Resist.Magic.Spirit"),
		FString("Spirit damage resistance")
	);
	Instance.Effect_Resist_Magic_Sun = TagManager.AddNativeGameplayTag(
		FName("Effect.Resist.Magic.Sun"),
		FString("Sun damage resistance")
	);
	Instance.Effect_Resist_Magic_Tech = TagManager.AddNativeGameplayTag(
		FName("Effect.Resist.Magic.Tech"),
		FString("Tech damage resistance")
	);
	Instance.Effect_Resist_Physical = TagManager.AddNativeGameplayTag(
		FName("Effect.Resist.Physical"),
		FString("Physical damage resistance")
	);
	Instance.Effect_Debuff = TagManager.AddNativeGameplayTag(
		FName("Effect.Debuff"),
		FString("Root tag for all debuff related tags")
	);
	Instance.Effect_Debuff_Type = TagManager.AddNativeGameplayTag(
		FName("Effect.Debuff.Type"),
		FString("Root tag for all types of debuffs")
	);
	Instance.Effect_Debuff_Type_Burn = TagManager.AddNativeGameplayTag(
		FName("Effect.Debuff.Type.Burn"),
		FString("Burns target for damage for a duration")
	);
	Instance.Effect_Debuff_Type_Shock = TagManager.AddNativeGameplayTag(
		FName("Effect.Debuff.Type.Shock"),
		FString("Shocks target for damage for a duration")
	);
	Instance.Effect_Debuff_Type_Curse = TagManager.AddNativeGameplayTag(
		FName("Effect.Debuff.Type.Curse"),
		FString("Curse target for damage for a duration")
	);
	Instance.Effect_Debuff_Type_Bleed = TagManager.AddNativeGameplayTag(
		FName("Effect.Debuff.Type.Bleed"),
		FString("Bleed target for damage for a duration")
	);
	Instance.Effect_Debuff_Type_Staggered = TagManager.AddNativeGameplayTag(FName("Effect.Debuff.Type.Staggered"), FString("Staggered status effect"));

	Instance.Effect_Debuff_Stat = TagManager.AddNativeGameplayTag(
		FName("Effect.Debuff.Stat"),
		FString("Root tag for debuff stat tags")
	);
	Instance.Effect_Debuff_Stat_Chance = TagManager.AddNativeGameplayTag(
		FName("Effect.Debuff.Stat.Chance"),
		FString("Effect.Debuff chance")
	);
	Instance.Effect_Debuff_Stat_Damage = TagManager.AddNativeGameplayTag(
		FName("Effect.Debuff.Stat.Damage"),
		FString("Effect.Debuff Damage")
	);
	Instance.Effect_Debuff_Stat_Frequency = TagManager.AddNativeGameplayTag(
		FName("Effect.Debuff.Stat.Frequency"),
		FString("Effect.Debuff Frequency")
	);
	Instance.Effect_Debuff_Stat_Duration = TagManager.AddNativeGameplayTag(
		FName("Effect.Debuff.Stat.Duration"),
		FString("Effect.Debuff Duration")
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
	Instance.Abilities_Form = TagManager.AddNativeGameplayTag(FName("Abilities.Form"));
	Instance.Abilities_Form_Barbarian = TagManager.AddNativeGameplayTag(FName("Abilities.Form.Barbarian"));
	Instance.Abilities_Form_Egyptian = TagManager.AddNativeGameplayTag(FName("Abilities.Form.Egyptian"));
	Instance.Abilities_Form_Futureman = TagManager.AddNativeGameplayTag(FName("Abilities.Form.Futureman"));
	Instance.Abilities_Form_Highlander = TagManager.AddNativeGameplayTag(FName("Abilities.Form.Highlander"));
	Instance.Abilities_Form_Hippie = TagManager.AddNativeGameplayTag(FName("Abilities.Form.Hippie"));
	Instance.Abilities_Form_Knight = TagManager.AddNativeGameplayTag(FName("Abilities.Form.Knight"));
	Instance.Abilities_Form_Native = TagManager.AddNativeGameplayTag(FName("Abilities.Form.Native"));
	Instance.Abilities_Form_Roman = TagManager.AddNativeGameplayTag(FName("Abilities.Form.Roman"));
	Instance.Abilities_Barbarian = TagManager.AddNativeGameplayTag(FName("Abilities.Barbarian"));
	Instance.Abilities_Egyptian = TagManager.AddNativeGameplayTag(FName("Abilities.Egyptian"));
	Instance.Abilities_Futureman = TagManager.AddNativeGameplayTag(FName("Abilities.Futureman"));
	Instance.Abilities_Highlander = TagManager.AddNativeGameplayTag(FName("Abilities.Highlander"));
	Instance.Abilities_Hippie = TagManager.AddNativeGameplayTag(FName("Abilities.Hippie"));
	Instance.Abilities_Knight = TagManager.AddNativeGameplayTag(FName("Abilities.Knight"));
	Instance.Abilities_Native = TagManager.AddNativeGameplayTag(FName("Abilities.Native"));
	Instance.Abilities_Roman = TagManager.AddNativeGameplayTag(FName("Abilities.Roman"));
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
	Instance.Abilities_Other_Aim = TagManager.AddNativeGameplayTag(FName("Abilities.Other.Aim"), FString("Aim ability tag"));
	Instance.Abilities_Other_LockOn = TagManager.AddNativeGameplayTag(FName("Abilities.Other.LockOn"), FString("LockOn ability tag"));
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
	Instance.Abilities_Parameters_CanBeBlocked = TagManager.AddNativeGameplayTag(FName("Abilities.Parameters.CanBeBlocked"), FString("Denotes whether the ability can be blocked"));
	Instance.Abilities_Parameters_CanBeParried = TagManager.AddNativeGameplayTag(FName("Abilities.Parameters.CanBeParried"), FString("Denotes whether the ability can be parried"));
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
	Instance.Event_Montage_Common_BlockAttack = TagManager.AddNativeGameplayTag(FName("Event.Montage.Common.BlockAttack"), FString("Indicates that the target successfully blocked an attack"));
	Instance.Event_Montage_Common_Blocked = TagManager.AddNativeGameplayTag(FName("Event.Montage.Common.Blocked"), FString("Indicates that the target's attack was blocked by the instigator"));
	Instance.Event_Montage_Common_ParryAttack = TagManager.AddNativeGameplayTag(FName("Event.Montage.Common.ParryAttack"), FString("Indicates that the target successfully parried an attack"));
	Instance.Event_Montage_Common_Staggered = TagManager.AddNativeGameplayTag(FName("Event.Montage.Common.Staggered"), FString("Indicates that the target was staggered by the instigator"));
	/** Cooldown Tags **/
	Instance.Cooldown_Form = TagManager.AddNativeGameplayTag(
		FName("Cooldown.Form"),
		FString("Change Form ability cooldown tag")
	);
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
	Instance.Combat_Socket_Weapon_Tip = TagManager.AddNativeGameplayTag(
		FName("Combat.Socket.Weapon.Tip"),
		FString("Weapon Tip Socket")
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
	Instance.Player_Block_Jump = TagManager.AddNativeGameplayTag(
		FName("Player.Block.Jump"),
		FString("Blocks player jumping")
	);
	Instance.Player_Block_Movement = TagManager.AddNativeGameplayTag(
		FName("Player.Block.Movement"),
		FString("Blocks player movement")
	);
	Instance.Player_Block_Rotation = TagManager.AddNativeGameplayTag(
		FName("Player.Block.Rotation"),
		FString("Blocks player rotation")
	);
	Instance.Player_Block_Interaction = TagManager.AddNativeGameplayTag(
		FName("Player.Block.Interaction"),
		FString("Blocks player's ability to interact with the environment")
	);
	Instance.Player_Block_ChangeForm = TagManager.AddNativeGameplayTag(
		FName("Player.Block.ChangeForm"),
		FString("Blocks player's ability to change form")
	);
	Instance.Player_Block_Ability_Offensive = TagManager.AddNativeGameplayTag(
		FName("Player.Block.Ability.Offensive"),
		FString("Blocks player offensive abilities")
	);
	Instance.Player_Block_Aim = TagManager.AddNativeGameplayTag(FName("Player.Block.Aim"), FString("Blocks player aim"));
	Instance.Player_Block_LockOn = TagManager.AddNativeGameplayTag(FName("Player.Block.LockOn"), FString("Blocks player lock on ability"));
	Instance.Player_Block_Look = TagManager.AddNativeGameplayTag(FName("Player.Block.Look"), FString("Blocks player look ability"));
	Instance.Player_Block_MotionWarpingUpdate = TagManager.AddNativeGameplayTag(FName("Player.Block.MotionWarpingUpdate"), FString("Blocks player from updating its motion warping target"));
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
	Instance.Player_Equipped_Weapon_Futureman_Blaster = TagManager.AddNativeGameplayTag(
		FName("Player.Equipped.Weapon.Futureman.Blaster"),
		FString("Player has the Futureman's Blaster weapon equipped")
	);
	Instance.Player_Form = TagManager.AddNativeGameplayTag(FName("Player.Form"), FString("Player form root tag"));
	Instance.Player_Form_Barbarian = TagManager.AddNativeGameplayTag(
		FName("Player.Form.Barbarian"),
		FString("Barbarian form tag")
	);
	Instance.Player_Form_Egyptian = TagManager.AddNativeGameplayTag(
		FName("Player.Form.Egyptian"),
		FString("Egyptian form tag")
	);
	Instance.Player_Form_Futureman = TagManager.AddNativeGameplayTag(
		FName("Player.Form.Futureman"),
		FString("Futureman form tag")
	);
	Instance.Player_Form_Highlander = TagManager.AddNativeGameplayTag(
		FName("Player.Form.Highlander"),
		FString("Highlander form tag")
	);
	Instance.Player_Form_Hippie = TagManager.AddNativeGameplayTag(
		FName("Player.Form.Hippie"),
		FString("Hippie form tag")
	);
	Instance.Player_Form_Knight = TagManager.AddNativeGameplayTag(
		FName("Player.Form.Knight"),
		FString("Knight form tag")
	);
	Instance.Player_Form_Native = TagManager.AddNativeGameplayTag(
		FName("Player.Form.Native"),
		FString("Native form tag")
	);
	Instance.Player_Form_Roman = TagManager.AddNativeGameplayTag(FName("Player.Form.Roman"), FString("Roman form tag"));
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
	Instance.Item_Type_Equipment_Weapon
		= TagManager.AddNativeGameplayTag(
			FName("Item.Type.Equipment.Weapon"),
			FString("Weapon root tag")
		);
	Instance.Item_Type_Equipment_Weapon_Futureman_Blaster
		= TagManager.AddNativeGameplayTag(
			FName("Item.Type.Equipment.Weapon.Futureman.Blaster"),
			FString("Blaster - used by the Futureman")
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
	Instance.Message_Default_Item_Pickup = TagManager.AddNativeGameplayTag(
		FName("Message.Default.Item.Pickup"),
		FString("Pickup item message tag")
	);
	Instance.Message_Default_Item_Use = TagManager.AddNativeGameplayTag(
		FName("Message.Default.Item.Use"),
		FString("Use item message tag")
	);

	Instance.DamageTypes.Add(Instance.Effect_Damage_Magic_Arcane);
	Instance.DamageTypes.Add(Instance.Effect_Damage_Magic_Dark);
	Instance.DamageTypes.Add(Instance.Effect_Damage_Magic_Fire);
	Instance.DamageTypes.Add(Instance.Effect_Damage_Magic_Lightning);
	Instance.DamageTypes.Add(Instance.Effect_Damage_Physical);
	Instance.DamageTypes.Add(Instance.Effect_Damage_Magic_Psych);
	Instance.DamageTypes.Add(Instance.Effect_Damage_Magic_Spirit);
	Instance.DamageTypes.Add(Instance.Effect_Damage_Magic_Sun);
	Instance.DamageTypes.Add(Instance.Effect_Damage_Magic_Tech);
	Instance.DamageTypeToResistanceType.Add(Instance.Effect_Damage_Physical, Instance.Effect_Resist_Physical);
	Instance.DamageTypeToResistanceType.Add(Instance.Effect_Damage_Magic_Arcane, Instance.Effect_Resist_Magic_Arcane);
	Instance.DamageTypeToResistanceType.Add(Instance.Effect_Damage_Magic_Dark, Instance.Effect_Resist_Magic_Dark);
	Instance.DamageTypeToResistanceType.Add(Instance.Effect_Damage_Magic_Fire, Instance.Effect_Resist_Magic_Fire);
	Instance.DamageTypeToResistanceType.Add(
		Instance.Effect_Damage_Magic_Lightning,
		Instance.Effect_Resist_Magic_Lightning
	);
	Instance.DamageTypeToResistanceType.Add(Instance.Effect_Damage_Magic_Psych, Instance.Effect_Resist_Magic_Psych);
	Instance.DamageTypeToResistanceType.Add(Instance.Effect_Damage_Magic_Spirit, Instance.Effect_Resist_Magic_Spirit);
	Instance.DamageTypeToResistanceType.Add(Instance.Effect_Damage_Magic_Sun, Instance.Effect_Resist_Magic_Sun);
	Instance.DamageTypeToResistanceType.Add(Instance.Effect_Damage_Magic_Tech, Instance.Effect_Resist_Magic_Tech);
	Instance.DebuffTypes.Add(Instance.Effect_Debuff_Type_Bleed);
	Instance.DebuffTypes.Add(Instance.Effect_Debuff_Type_Burn);
	Instance.DebuffTypes.Add(Instance.Effect_Debuff_Type_Curse);
	Instance.DebuffTypes.Add(Instance.Effect_Debuff_Type_Shock);
}

TArray<FGameplayTag> FElectricCastleGameplayTags::GetDamageTypes() const
{
	return DamageTypes;
}

TArray<FGameplayTag> FElectricCastleGameplayTags::GetDebuffTypes() const
{
	return DebuffTypes;
}

FGameplayTag FElectricCastleGameplayTags::GetDamageTypeResistanceTag(const FGameplayTag& DamageTypeTag) const
{
	if (DamageTypeToResistanceType.Contains(DamageTypeTag))
	{
		return DamageTypeToResistanceType[DamageTypeTag];
	}
	UE_LOG(
		LogElectricCastle,
		Warning,
		TEXT("[FElectricCastleGameplayTags] No resistance tag for damage type: %s"),
		*DamageTypeTag.ToString()
	);
	return FGameplayTag::EmptyTag;
}
