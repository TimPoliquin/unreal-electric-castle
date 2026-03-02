// Copyright Alien Shores


#include "Actor/Puzzle/Action/PuzzleAction.h"

#include "ElectricCastle/ElectricCastleLogChannels.h"

void UPuzzleAction::Initialize_Implementation(AActor* InOwner, UPuzzleManagerComponent* InPuzzleManager)
{
	Owner = InOwner;
	PuzzleManager = InPuzzleManager;
}

bool UPuzzleAction::ShouldExecute_Implementation() const
{
	return true;
}

void UPuzzleAction::Execute_Implementation() const
{
	UE_LOG(LogElectricCastle, Error, TEXT("[%s] Must implement Execute_Implementation!"), *GetClass()->GetName())
}

AActor* UPuzzleAction::GetOwner() const
{
	return Owner;
}

FString UPuzzleAction::GetOwnerName() const
{
	return Owner ? Owner->GetName() : TEXT("NULLPTR");
}

UPuzzleManagerComponent* UPuzzleAction::GetPuzzleManager() const
{
	return PuzzleManager;
}

UWorld* UPuzzleAction::GetWorld() const
{
	return GetOwner() ? GetOwner()->GetWorld() : Super::GetWorld();
}
