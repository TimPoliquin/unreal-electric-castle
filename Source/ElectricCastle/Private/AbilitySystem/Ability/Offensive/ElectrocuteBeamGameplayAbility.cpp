// Copyright Alien Shores


#include "AbilitySystem/Ability/Offensive/ElectrocuteBeamGameplayAbility.h"

FString UElectrocuteBeamGameplayAbility::GetDescription_Implementation(const int32 AbilityLevel) const
{
	const float ManaCost = GetManaCost(AbilityLevel);
	const float Cooldown = GetCooldown(AbilityLevel);
	const int32 Damage = GetDamageAtLevel(AbilityLevel);
	return FString::Printf(
		TEXT(
			// Title
			"" RICH_TITLE("%s") "\n\n"
			// Details
			RICH_SMALL("Level: ") RICH_LEVEL("%d") "\n" // Level
			RICH_SMALL("Mana Cost: ") RICH_MANA_COST("%.1f") "\n" // Mana Cost
			RICH_SMALL("Cooldown: ") RICH_COOLDOWN("%.1f") "\n\n" // Cooldown
			// Description
			RICH_DEFAULT("Emits a beam of lightning, repeatedly causing ") RICH_DAMAGE("%d")
			RICH_DEFAULT(
				" lightning damage with a chance to shock."
			) "\n\n"
		),
		*GetAbilityName(),
		AbilityLevel,
		FMath::Abs(ManaCost),
		FMath::Abs(Cooldown),
		Damage
	);
}
