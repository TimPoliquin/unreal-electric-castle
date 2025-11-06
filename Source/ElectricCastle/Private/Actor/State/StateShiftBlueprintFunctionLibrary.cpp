// Copyright Alien Shores


#include "Actor/State/StateShiftBlueprintFunctionLibrary.h"

#include "Actor/State/StateShiftInterface.h"
#include "Actor/State/StateShiftComponent.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"

void UStateShiftBlueprintFunctionLibrary::SetActorVisibility(AActor* Actor, const bool bEnableVisibility)
{
	if (IsValid(Actor))
	{
		Actor->SetActorHiddenInGame(!bEnableVisibility);
	}
}

void UStateShiftBlueprintFunctionLibrary::HandleStateShift(AActor* Actor, const EStateShiftState NewState)
{
	if (!IsValid(Actor) || !Actor->Implements<UStateShiftInterface>())
	{
		UE_LOG(
			LogElectricCastle,
			Warning,
			TEXT("[UStateShiftBlueprintFunctionLibrary::HandleStateShift] Actor is not a UStateShiftInterface! %s"),
			Actor ? *Actor->GetName() : TEXT("NULL")
		);
		return;
	}
	if (UStateShiftComponent* StateShiftComponent = IStateShiftInterface::GetStateShiftComponent(Actor))
	{
		if (!StateShiftComponent->ShouldChangeState(NewState))
		{
			UE_LOG(
				LogElectricCastle,
				Warning,
				TEXT("[%s] Actor does not need to change states to %s"),
				*Actor->GetName(),
				*UEnum::GetValueAsString(NewState)
			)
			return;
		}
		StateShiftComponent->SetCurrentState(NewState);
	}
	else
	{
		UE_LOG(
			LogElectricCastle,
			Warning,
			TEXT("[%s] Actor does not have a state shift component"),
			*Actor->GetName()
		)
	}
}
