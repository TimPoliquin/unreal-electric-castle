// Copyright Alien Shores


#include "AbilitySystem/Data/AttributeInfo.h"

#include "ElectricCastle/ElectricCastleLogChannels.h"

FAttributeInfoRow UAttributeInfo::FindAttributeInfoForTag(
	const FGameplayTag& AttributeTag,
	bool bLogNotFound
) const
{
	for (const FAttributeInfoRow& Info : AttributeInformation)
	{
		if (Info.Tag.MatchesTagExact(AttributeTag))
		{
			return Info;
		}
	}
	if (bLogNotFound)
	{
		UE_LOG(
			LogElectricCastle,
			Error,
			TEXT("Can't find Info for Attribute Tag [%s] on AttributeInfo [%s]."),
			*AttributeTag.ToString(),
			*GetNameSafe(this)
		);
	}
	return FAttributeInfoRow();
}
