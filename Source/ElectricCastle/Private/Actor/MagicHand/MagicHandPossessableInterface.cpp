// Copyright Alien Shores


#include "Actor/MagicHand/MagicHandPossessableInterface.h"


// Add default functionality here for any IMagicHandPossessableInterface functions that are not pure virtual.
USceneComponent* IMagicHandPossessableInterface::GetMagicHandAttachComponent(AActor* Actor, FName& AttachBoneName)
{
	if (IsValid(Actor) && Actor->Implements<UMagicHandPossessableInterface>())
	{
		USceneComponent* AttachComponent = Execute_GetMagicHandAttachComponent(Actor, AttachBoneName);
		return AttachComponent ? AttachComponent : Actor->GetRootComponent();
	}
	return nullptr;
}

bool IMagicHandPossessableInterface::CanBePossessedByMagicHand(const UObject* Actor)
{
	if (IsValid(Actor) && Actor->Implements<UMagicHandPossessableInterface>())
	{
		return Execute_CanBePossessedByMagicHand(Actor);
	}
	return false;
}

void IMagicHandPossessableInterface::OnMagicHandPossess(UObject* Actor)
{
	if (IsValid(Actor) && Actor->Implements<UMagicHandPossessableInterface>())
	{
		Execute_OnMagicHandPossess(Actor);
	}
}

void IMagicHandPossessableInterface::OnMagicHandUnpossess(UObject* Actor)
{
	if (IsValid(Actor) && Actor->Implements<UMagicHandPossessableInterface>())
	{
		Execute_OnMagicHandUnpossess(Actor);
	}
}
