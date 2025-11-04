#include "Utils/TagUtils.h"

#include "ElectricCastle/ElectricCastleLogChannels.h"

bool TagUtils::HasAnyTag(const AActor* Actor, const TArray<FName>& Tags)
{
	if (!Actor || !IsValid(Actor))
	{
		return false;
	}
	for (const FName& Tag : Tags)
	{
		if (!Tag.IsNone() && Actor->ActorHasTag(Tag))
		{
			return true;
		}
	}
	return false;
}
