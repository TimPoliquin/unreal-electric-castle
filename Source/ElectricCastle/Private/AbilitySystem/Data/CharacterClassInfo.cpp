// Copyright Alien Shores


#include "AbilitySystem/Data/CharacterClassInfo.h"

FCharacterClassDefaultInfo UCharacterClassInfo::GetClassDefaultInfo(const ECharacterClass CharacterClass) const
{
	return CharacterClassInformation.FindChecked(CharacterClass);
}

float UCharacterClassInfo::GetArmorPenetrationCoefficient(const int Level) const
{
	return DamageCalculationCoefficients->FindCurve(
		FName("ArmorPenetration"),
		FString()
	)->Eval(Level);
}

float UCharacterClassInfo::GetEffectiveArmorCoefficient(const int Level) const
{
	return DamageCalculationCoefficients->FindCurve(
		FName("EffectiveArmor"),
		FString()
	)->Eval(Level);
}

float UCharacterClassInfo::GetCriticalHitResistanceCoefficient(const int Level) const
{
	return DamageCalculationCoefficients->FindCurve(
		FName("CriticalHitResistance"),
		FString()
	)->Eval(Level);
}

int32 UCharacterClassInfo::GetXPReward(const ECharacterClass& CharacterClass, const int32 Level) const
{
	return static_cast<int>(CharacterClassInformation[CharacterClass].XPCurve.GetValueAtLevel(Level));
}

bool CharacterClassUtils::IsRangedAttacker(ECharacterClass CharacterClass)
{
	switch (CharacterClass)
	{
	case ECharacterClass::Warrior:
		return false;
	case ECharacterClass::Elementalist:
		return true;
	case ECharacterClass::Ranger:
		return true;
	}
	return false;
}
