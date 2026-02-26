// Copyright Alien Shores


#include "Cinematic/Metadata/CinematicSequenceMetaData.h"

void UCinematicSequenceMetadata::CopyFrom(const UCinematicSequenceMetadata* Source)
{
	TypeTags = Source->TypeTags;
	ReactionTriggerTags = Source->ReactionTriggerTags;
	Description = Source->Description;
	bSkippable = Source->bSkippable;
	PlayerRelocationMode = Source->PlayerRelocationMode;
	RelocatePlayerLocation = Source->RelocatePlayerLocation;
}
