// Copyright Alien Shores


#include "AbilitySystem/Ability/Passive/HaloOfProtectionGameplayAbility.h"

FString UHaloOfProtectionGameplayAbility::GetDescription_Implementation(const int32 AbilityLevel) const
{
	const float ManaCost = GetManaCost(AbilityLevel);
	const float Cooldown = GetCooldown(AbilityLevel);
	const float Protection = (GetProtectionAtLevel(AbilityLevel) - 1) * 100.f;
	return FString::Printf(
		TEXT(
			// Title
			"" RICH_TITLE("%s") "\n\n"
			// Details
			RICH_SMALL("Level: ") RICH_LEVEL("%d") "\n" // Level
			RICH_SMALL("Mana Cost: ") RICH_MANA_COST("%.1f") "\n" // Mana Cost
			RICH_SMALL("Cooldown: ") RICH_COOLDOWN("%.1f") "\n\n" // Cooldown
			// Description
			RICH_DEFAULT("Surrounds the player with a halo of protection, increasing Armor effectiveness by "
			) RICH_BUFF("%d%%")
			RICH_DEFAULT(
				"."
			) "\n\n"
		),
		*GetAbilityName(),
		AbilityLevel,
		FMath::Abs(ManaCost),
		FMath::Abs(Cooldown),
		FMath::RoundToInt(Protection)
	);
}

float UHaloOfProtectionGameplayAbility::GetProtectionAtLevel(int32 Level) const
{
	if (ProtectionConfig.IsValid())
	{
		return ProtectionConfig.GetValueAtLevel(Level);
	}
	return -999;
}
