// Copyright Alien Shores


#include "Actor/Highlight/HighlightConfig.h"

FHighlightConfigRow UHighlightConfig::GetHighlightConfigByHighlightType(const EHighlightType HighlightType) const
{
	if (HighlightTypes.Contains(HighlightType))
	{
		return HighlightTypes[HighlightType];
	}
	return FHighlightConfigRow();
}
