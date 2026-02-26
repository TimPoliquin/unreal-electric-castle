// Copyright Alien Shores


#include "Actor/Cinematic/Actions/HideActorsCinematicEventAction.h"

#include "Cinematic/Context/CinematicContextHandle.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"

void UHideActorsCinematicEventAction::GetActorsToHide_Implementation(TArray<AActor*>& OutActors) const
{
	UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Must implement ActorsToHide_Implementation!"), *GetClass()->GetName());
}

void UHideActorsCinematicEventAction::Execute_Implementation(const UCinematicContextHandle* ContextHandle) const
{
	TArray<AActor*> ActorsToHide;
	TArray<AActor*> ChildActors;
	GetActorsToHide(ActorsToHide);
	if (bDebug)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Hiding %d actors"), *GetClass()->GetName(), ActorsToHide.Num())
	}
	for (AActor* Actor : ActorsToHide)
	{
		if (!IsValid(Actor))
		{
			if (bDebug)
			{
				UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Actor is invalid"), *GetClass()->GetName());
			}
			continue;
		}
		HideActor(Actor, ContextHandle);
		if (bApplyRelocation && ContextHandle->ShouldRelocate())
		{
			RelocateActor(Actor, ContextHandle);
		}
		if (bHideChildActors)
		{
			Actor->GetAttachedActors(ChildActors, true, true);
			Actor->GetAllChildActors(ChildActors, true);
			for (AActor* Child : ChildActors)
			{
				HideActor(Child, ContextHandle);
			}
		}
	}
}

void UHideActorsCinematicEventAction::HideActor(AActor* Actor, const UCinematicContextHandle* ContextHandle) const
{
	if (IsValid(Actor) && !Actor->IsHidden())
	{
		ContextHandle->AddRestoreLambda(Actor, Actor->IsHidden(), [this](AActor* Actor, const bool& bInValue)
		{
			UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Restoring actor %s to hidden state: %d"), *GetClass()->GetName(), *Actor->GetName(), bInValue);
			Actor->SetActorHiddenInGame(bInValue);
		});
		Actor->SetActorHiddenInGame(true);
		if (bDebug)
		{
			UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Hiding actor %s"), *GetClass()->GetName(), *Actor->GetName());
		}
	}
	else if (bDebug)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Actor %s is already hidden"), *GetClass()->GetName(), Actor ? *Actor->GetName() : *FString("NULLPTR"));
	}
}

void UHideActorsCinematicEventAction::RelocateActor(AActor* Actor, const UCinematicContextHandle* ContextHandle) const
{
	if (IsValid(Actor))
	{
		ContextHandle->AddRestoreLambda(Actor, Actor->GetActorLocation(), [](AActor* Actor, const FVector& InValue) { Actor->SetActorLocation(InValue); });
		Actor->SetActorLocation(ContextHandle->GetRelocateLocation(), false);
	}
	else if (bDebug)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Actor %s is invalid"), *GetClass()->GetName(), Actor ? *Actor->GetName() : *FString("NULLPTR"));
	}
}
