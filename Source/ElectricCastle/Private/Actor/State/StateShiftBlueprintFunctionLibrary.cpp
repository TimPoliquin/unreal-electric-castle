// Copyright Alien Shores


#include "Actor/State/StateShiftBlueprintFunctionLibrary.h"

#include "Actor/State/StateShiftActor.h"
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
	if (!IsValid(Actor) || !Actor->Implements<UStateShiftActor>())
	{
		UE_LOG(
			LogElectricCastle,
			Warning,
			TEXT("[UStateShiftBlueprintFunctionLibrary::HandleStateShift] Actor is not a UStateShiftActor! %s"),
			Actor ? *Actor->GetName() : TEXT("NULL")
		);
		return;
	}
	if (UStateShiftComponent* StateShiftComponent = IStateShiftActor::Execute_GetStateShiftComponent(Actor))
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
		for (EStateShiftReactionType Reaction : StateShiftComponent->GetReactions(NewState))
		{
			UE_LOG(
				LogElectricCastle,
				Warning,
				TEXT("[%s] Actor executing reaction: %s"),
				*Actor->GetName(),
				*UEnum::GetValueAsString(Reaction)
			)
			switch (Reaction)
			{
			case EStateShiftReactionType::Custom:
				IStateShiftActor::Execute_StateShiftReaction_Custom(Actor);
				break;
			case EStateShiftReactionType::Visibility_Show:
				IStateShiftActor::Execute_StateShiftReaction_Visibility_Show(Actor);
				break;
			case EStateShiftReactionType::Visibility_Hide:
				IStateShiftActor::Execute_StateShiftReaction_Visibility_Hide(Actor);
				break;
			case EStateShiftReactionType::Visibility_FadeIn:
				IStateShiftActor::Execute_StateShiftReaction_Visibility_FadeIn(Actor);
				break;
			case EStateShiftReactionType::Visibility_FadeOut:
				IStateShiftActor::Execute_StateShiftReaction_Visibility_FadeOut(Actor);
				break;
			case EStateShiftReactionType::Collision_Enable:
				IStateShiftActor::Execute_StateShiftReaction_Collision_Enable(Actor);
				break;
			case EStateShiftReactionType::Collision_Disable:
				IStateShiftActor::Execute_StateShiftReaction_Collision_Disable(Actor);
				break;
			default:
				break;
			}
		}
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
