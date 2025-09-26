// Copyright Alien Shores


#include "AbilitySystem/Ability/Passive/LeachGameplayAbility.h"

#include "AbilitySystem/ElectricCastleAbilitySystemComponent.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "Tags/ElectricCastleGameplayTags.h"

FString ULeachGameplayAbility::GetDescription_Implementation(const int32 AbilityLevel) const
{
	const float ManaCost = GetManaCost(AbilityLevel);
	const float Cooldown = GetCooldown(AbilityLevel);
	const float LeachPercentage = GetLeachAmountAtLevel(AbilityLevel) * 100.f;
	return FString::Printf(
		TEXT(
			// Title
			"" RICH_TITLE("%s") "\n\n"
			// Details
			RICH_SMALL("Level: ") RICH_LEVEL("%d") "\n" // Level
			RICH_SMALL("Mana Cost: ") RICH_MANA_COST("%.1f") "\n" // Mana Cost
			RICH_SMALL("Cooldown: ") RICH_COOLDOWN("%.1f") "\n\n" // Cooldown
			// Description
			RICH_DEFAULT("Absorbs ") RICH_BUFF("%d%%") RICH_DEFAULT(" of damage dealt as regenerated %s.")
			"\n\n"
		),
		*GetAbilityName(),
		AbilityLevel,
		FMath::Abs(ManaCost),
		FMath::Abs(Cooldown),
		FMath::RoundToInt(LeachPercentage),
		*LeachAttribute
	);
}

float ULeachGameplayAbility::GetLeachMagnitude(const FOnAbilitySystemOutgoingDamagePayload& Payload) const
{
	const float OutgoingDamage = Payload.OutgoingDamage;
	const float LeachAmount = GetLeachAmountAtLevel(GetAbilityLevel());
	return OutgoingDamage * LeachAmount;
}

FActiveGameplayEffectHandle ULeachGameplayAbility::ApplyLeachEffect(
	const FOnAbilitySystemOutgoingDamagePayload& Payload
)
{
	return UElectricCastleAbilitySystemLibrary::ApplyBasicGameplayEffectWithMagnitude(
		GetAvatarActorFromActorInfo(),
		LeachEffect,
		GetAbilityLevel(),
		FElectricCastleGameplayTags::Get().Effect_Magnitude,
		GetLeachMagnitude(Payload)
	);
}

float ULeachGameplayAbility::GetLeachAmountAtLevel(const int32 Level) const
{
	if (LeachConfig.IsValid())
	{
		return LeachConfig.GetValueAtLevel(Level);
	}
	return -999;
}
