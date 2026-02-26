// Copyright Alien Shores


#include "Actor/Cinematic/Actions/DisableTickCinematicEventAction.h"

#include "Cinematic/Context/CinematicContextHandle.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Tags/ElectricCastleGameplayTags.h"

UDisableTickCinematicEventAction::UDisableTickCinematicEventAction()
{
	Triggers.AddTag(FElectricCastleGameplayTags::Get().Cinematic_Reaction_Trigger_HideNonCinematicActors);
}

void UDisableTickCinematicEventAction::GetActorsToDisableTick_Implementation(TArray<AActor*>& OutActors) const
{
	// do nothing
	UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Must implement GetActorsToDisableTick!"), *GetClass()->GetName());
}

void UDisableTickCinematicEventAction::Execute_Implementation(const UCinematicContextHandle* ContextHandle) const
{
	TArray<AActor*> ActorsToDisable;
	TArray<AActor*> ChildActors;
	GetActorsToDisableTick(ActorsToDisable);
	if (bDebug)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Disabling tick for %d actors"), *GetClass()->GetName(), ActorsToDisable.Num())
	}
	for (AActor* Actor : ActorsToDisable)
	{
		if (!IsValid(Actor))
		{
			if (bDebug)
			{
				UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Actor is invalid"), *GetClass()->GetName());
			}
			continue;
		}
		DisableTick(Actor, ContextHandle);
		if (bDisableChildActors)
		{
			Actor->GetAttachedActors(ChildActors, true, true);
			Actor->GetAllChildActors(ChildActors, true);
			for (AActor* Child : ChildActors)
			{
				DisableTick(Child, ContextHandle);
			}
		}
	}
}

void UDisableTickCinematicEventAction::DisableTick(AActor* Actor, const UCinematicContextHandle* ContextHandle) const
{
	if (IsValid(Actor) && Actor->IsActorTickEnabled())
	{
		ContextHandle->AddRestoreLambda(Actor, Actor->IsActorTickEnabled(), [this](AActor* Actor, const bool& bInValue)
		{
			UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Restoring actor %s tick state: %d"), *GetClass()->GetName(), *Actor->GetName(), bInValue);
			Actor->SetActorTickEnabled(bInValue);
		});
		Actor->SetActorTickEnabled(false);
		if (bDebug)
		{
			UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Disabling actor tick: %s"), *GetClass()->GetName(), *Actor->GetName());
		}
	}
	else if (bDebug)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Actor %s tick is already disabled"), *GetClass()->GetName(), Actor ? *Actor->GetName() : *FString("NULLPTR"));
	}
}
