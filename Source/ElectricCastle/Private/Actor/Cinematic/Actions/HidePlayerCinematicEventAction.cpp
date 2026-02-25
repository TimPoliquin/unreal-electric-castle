// Copyright Alien Shores


#include "Actor/Cinematic/Actions/HidePlayerCinematicEventAction.h"

#include "Character/ElectricCastlePlayerCharacter.h"
#include "Cinematic/Context/CinematicContextHandle.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Game/Subsystem/PlayerManager.h"
#include "Tags/ElectricCastleGameplayTags.h"

UHidePlayerCinematicEventAction::UHidePlayerCinematicEventAction()
{
	Triggers.AddTag(FElectricCastleGameplayTags::Get().Cinematic_Reaction_Trigger_HidePlayer);
}

void UHidePlayerCinematicEventAction::GetActorsToHide_Implementation(TArray<AActor*>& OutActors) const
{
	const UPlayerManager* PlayerManager = UPlayerManager::Get(GetOwner());
	if (!PlayerManager)
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[%s] PlayerManager not found for owner %s"), *GetClass()->GetName(), *GetOwnerName())
		return;
	}
	TArray<AElectricCastlePlayerCharacter*> PlayerCharacters;
	TArray<AActor*> ChildActors;
	PlayerManager->GetPlayerCharacters(PlayerCharacters);
	OutActors.Append(PlayerCharacters);
}
