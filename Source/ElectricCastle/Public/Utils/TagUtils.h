#pragma once

class TagUtils
{
public:
	static bool HasAnyTag(const AActor* Actor, const TArray<FName>& Tags);
};
