#include "Utils/TagUtils.h"

bool TagUtils::HasAnyTag(const AActor* Actor, const TArray<FName>& Tags)
{
	if (!IsValid(Actor))
	{
		return false;
	}
	for (const FName& Tag : Tags)
	{
		if (Actor->ActorHasTag(Tag))
		{
			return true;
		}
	}
	return false;
}
