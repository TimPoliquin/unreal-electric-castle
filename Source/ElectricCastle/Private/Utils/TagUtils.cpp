#include "Utils/TagUtils.h"

bool TagUtils::HasAnyTag(const AActor* Actor, const TArray<FName>& Tags)
{
	for (const FName& Tag : Tags)
	{
		if (Actor->ActorHasTag(Tag))
		{
			return true;
		}
	}
	return false;
}
