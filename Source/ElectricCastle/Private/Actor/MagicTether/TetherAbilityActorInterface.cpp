// Copyright Alien Shores


#include "Actor/MagicTether/TetherAbilityActorInterface.h"

#include "ElectricCastle/ElectricCastleLogChannels.h"


UTetherAbilityComponent* ITetherAbilityActorInterface::GetTetherAbilityComponent(const UObject* Actor)
{
	if (IsValid(Actor) && Actor->Implements<UTetherAbilityActorInterface>())
	{
		return Execute_GetTetherAbilityComponent(Actor);
	}
	UE_LOG(LogElectricCastle, Log, TEXT("[ITetherAbilityActorInterface] Actor %s does not implement ITetherAbilityActorInterface."), Actor ? *Actor->GetName() : *FString("NULLPTR"))
	return nullptr;
}
