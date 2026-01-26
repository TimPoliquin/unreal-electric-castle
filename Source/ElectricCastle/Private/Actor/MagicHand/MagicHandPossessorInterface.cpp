// Copyright Alien Shores


#include "Actor/MagicHand/MagicHandPossessorInterface.h"


// Add default functionality here for any IMagicHandPossessorInterface functions that are not pure virtual.
USceneComponent* IMagicHandPossessorInterface::GetMagicHandAttachComponent(AActor* Actor, FName& AttachBoneName)
{
	if (IsValid(Actor) && Actor->Implements<UMagicHandPossessorInterface>())
	{
		USceneComponent* AttachComponent = Execute_GetMagicHandAttachComponent(Actor, AttachBoneName);
		return AttachComponent ? AttachComponent : Actor->GetRootComponent();
	}
	return nullptr;
}
