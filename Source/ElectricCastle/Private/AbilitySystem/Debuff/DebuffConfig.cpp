// Copyright Alien Shores


#include "AbilitySystem/Debuff/DebuffConfig.h"

FDebuffConfigRow UDebuffConfig::GetDebuffConfigByDebuffTag(const FGameplayTag& DebuffTag) const
{
	const FDebuffConfigRow* ConfigRow = DebuffConfigRows.FindByPredicate([DebuffTag](const FDebuffConfigRow& Row)
	{
		return Row.DebuffTag == DebuffTag;
	});
	if (ConfigRow)
	{
		return *ConfigRow;
	}
	return FDebuffConfigRow();
}
