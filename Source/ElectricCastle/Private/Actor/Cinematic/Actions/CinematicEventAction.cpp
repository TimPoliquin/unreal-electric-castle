// Copyright Alien Shores


#include "Actor/Cinematic/Actions/CinematicEventAction.h"

#include "Cinematic/Context/CinematicContextHandle.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"

bool UCinematicEventAction::ShouldExecute_Implementation(const UCinematicContextHandle* ContextHandle) const
{
	return ContextHandle->HasAnyTag(Triggers);
}

void UCinematicEventAction::Execute_Implementation(const UCinematicContextHandle* ContextHandle) const
{
	UE_LOG(LogElectricCastle, Error, TEXT("[%s] Must implement Execute_Implementation!"), *GetClass()->GetName());
}

void UCinematicEventAction::Initialize_Implementation(AActor* InOwner)
{
	Owner = InOwner;
}

AActor* UCinematicEventAction::GetOwner() const
{
	return Owner.Get();
}

FString UCinematicEventAction::GetOwnerName() const
{
	return Owner.IsValid() ? Owner->GetName() : TEXT("NULLPTR");
}

UWorld* UCinematicEventAction::GetWorld() const
{
	return Owner.IsValid() ? Owner->GetWorld() : nullptr;
}

AController* UCinematicEventAction::GetOwnerController() const
{
	if (GetOwner() && GetOwner()->IsA<AController>())
	{
		return Cast<AController>(GetOwner());
	}
	if (const APawn* Pawn = Cast<APawn>(GetOwner()))
	{
		return Pawn->GetController();
	}
	return nullptr;
}
