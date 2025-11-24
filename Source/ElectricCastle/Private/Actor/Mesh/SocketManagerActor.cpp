// Copyright Alien Shores


#include "Actor/Mesh/SocketManagerActor.h"

#include "ElectricCastle/ElectricCastleLogChannels.h"


// Add default functionality here for any ISocketManagerActor functions that are not pure virtual.
USocketManagerComponent* ISocketManagerActor::GetSocketManagerComponent(const UObject* Actor)
{
	if (!IsValid(Actor))
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[ISocketManagerActor] Unable to get component - Actor is invalid."))
		return nullptr;
	}
	if (Actor->Implements<USocketManagerActor>())
	{
		return Execute_GetSocketManagerComponent(Actor);
	}
	UE_LOG(LogElectricCastle, Warning, TEXT("[ISocketManagerActor] Actor %s does not implement ISocketManagerActor."), *Actor->GetName())
	return nullptr;
}
