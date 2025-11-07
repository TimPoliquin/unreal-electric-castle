// Copyright Alien Shores


#include "Actor/State/StateShiftComponent.h"

#include "Actor/State/StateShiftBlueprintFunctionLibrary.h"
#include "Net/UnrealNetwork.h"


UStateShiftComponent::UStateShiftComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UStateShiftComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UStateShiftComponent, CurrentState);
}

bool UStateShiftComponent::ShouldChangeState(const EStateShiftState NewState) const
{
	return NewState != CurrentState;
}

void UStateShiftComponent::ChangeState(const FStateShiftRequest& StateShiftRequest)
{
	if (StateShiftRequest.NewState != CurrentState)
	{
		const EStateShiftState OldState = CurrentState;
		CurrentState = StateShiftRequest.NewState;
		OnStateShiftStateChanged.Broadcast(FStateShiftStateChangedPayload::FromRequest(StateShiftRequest, OldState));
	}
}

TArray<EStateShiftReactionType> UStateShiftComponent::GetReactions(const EStateShiftState NewState) const
{
	switch (NewState)
	{
	case EStateShiftState::Normal:
		return StateShiftReactions.NormalReactions;
	case EStateShiftState::Psychedelic:
		return StateShiftReactions.PsychedelicReactions;
	default:
		return TArray<EStateShiftReactionType>();
	}
}
