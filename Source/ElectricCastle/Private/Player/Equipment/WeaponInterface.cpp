// Copyright Alien Shores


#include "Player/Equipment/WeaponInterface.h"

#include "ElectricCastle/ElectricCastleLogChannels.h"


// Add default functionality here for any IWeaponInterface functions that are not pure virtual.
void IWeaponInterface::EffectAdd_Lightning(UObject* Weapon)
{
	if (IsValid(Weapon) && Weapon->Implements<UWeaponInterface>())
	{
		Execute_EffectAdd_Lightning(Weapon);
		return;
	}
	UE_LOG(
		LogElectricCastle,
		Error,
		TEXT("IWeaponInterface::EffectAdd_Lightning: Object is not valid or is not a weapon")
	)
}

void IWeaponInterface::EffectRemove_Lightning(UObject* Weapon)
{
	if (IsValid(Weapon) && Weapon->Implements<UWeaponInterface>())
	{
		Execute_EffectRemove_Lightning(Weapon);
	}
}
