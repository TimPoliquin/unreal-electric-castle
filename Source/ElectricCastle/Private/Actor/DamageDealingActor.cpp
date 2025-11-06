// Copyright Alien Shores


#include "Actor/DamageDealingActor.h"

#include "ElectricCastle/ElectricCastleLogChannels.h"


// Add default functionality here for any IDamageDealingActor functions that are not pure virtual.
void IDamageDealingActor::ApplyDamageEffectParams(
	UObject* DamageDealingActor,
	const FDamageEffectParams& InDamageEffectParams
)
{
	if (!IsValid(DamageDealingActor))
	{
		return;
	}
	if (DamageDealingActor->Implements<UDamageDealingActor>())
	{
		Execute_ApplyDamageEffectParams(DamageDealingActor, InDamageEffectParams);
	}
	else
	{
		UE_LOG(
			LogElectricCastle,
			Warning,
			TEXT("[IDamageDealingActor::ApplyDamageEffectParams] %s does not implement IDamageDealingActor!"),
			*DamageDealingActor->GetName()
		);
	}
}
