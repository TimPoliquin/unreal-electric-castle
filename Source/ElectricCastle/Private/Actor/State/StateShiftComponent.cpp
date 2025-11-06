// Copyright Alien Shores


#include "Actor/State/StateShiftComponent.h"

#include "Actor/State/StateShiftActor.h"
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


void UStateShiftComponent::BeginPlay()
{
	Super::BeginPlay();
	IStateShiftActor::EnableStateShiftCollision(GetOwner());
	UStateShiftBlueprintFunctionLibrary::HandleStateShift(GetOwner(), CurrentState);
}

void UStateShiftComponent::HandleStateShift(EStateShiftState NewState)
{
	UStateShiftBlueprintFunctionLibrary::HandleStateShift(GetOwner(), NewState);
}

bool UStateShiftComponent::ShouldChangeState(const EStateShiftState NewState) const
{
	return NewState != CurrentState;
}

void UStateShiftComponent::SetCurrentState(EStateShiftState NewState)
{
	CurrentState = NewState;
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
