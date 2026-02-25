// Copyright Alien Shores


#include "Actor/Cinematic/Actions/UnpossessControlledPawnCinematicEventAction.h"

#include "Cinematic/Context/CinematicContextHandle.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Tags/ElectricCastleGameplayTags.h"

UUnpossessControlledPawnCinematicEventAction::UUnpossessControlledPawnCinematicEventAction()
{
	Triggers.AddTag(FElectricCastleGameplayTags::Get().Cinematic_Reaction_Trigger_HideNonCinematicActors);
}

void UUnpossessControlledPawnCinematicEventAction::Execute_Implementation(const UCinematicContextHandle* ContextHandle) const
{
	if (bDebug)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Executing"), *GetClass()->GetName());
	}
	if (AController* Controller = Cast<AController>(GetOwner()))
	{
		if (Controller->GetPawn())
		{
			if (bDebug)
			{
				UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Unpossessing controlled pawn %s"), *GetClass()->GetName(), *Controller->GetPawn()->GetName());
			}
			ContextHandle->AddRestoreLambda(Controller, Controller->GetPawn(), [](AController* Controller, APawn* Pawn) { Controller->Possess(Pawn); });
			Controller->UnPossess();
		}
	}
}
