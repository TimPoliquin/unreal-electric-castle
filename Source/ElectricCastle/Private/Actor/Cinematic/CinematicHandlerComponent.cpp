// Copyright Alien Shores


#include "Actor/Cinematic/CinematicHandlerComponent.h"

#include "Cinematic/Context/CinematicContextHandle.h"
#include "Cinematic/CinematicManager.h"


UCinematicHandlerComponent::UCinematicHandlerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCinematicHandlerComponent::InitializeComponent()
{
	Super::InitializeComponent();
	if (UCinematicManager* CinematicManager = UCinematicManager::Get(GetOwner()))
	{
		CinematicManager->OnCinematicBegin.AddUniqueDynamic(this, &UCinematicHandlerComponent::OnCinematicBegin);
		CinematicManager->OnCinematicEnd.AddUniqueDynamic(this, &UCinematicHandlerComponent::OnCinematicEnd);
	}
}

UCinematicHandlerComponent* UCinematicHandlerComponent::GetCinematicHandlerComponent_Implementation()
{
	return this;
}

bool UCinematicHandlerComponent::HandleCinematicEvent_Activate_Implementation(const UCinematicContextHandle* CinematicContextHandle)
{
	// no default action here - let the actor override it
	return ICinematicHandlerInterface::Activate(GetOwner(), CinematicContextHandle);
}

bool UCinematicHandlerComponent::HandleCinematicEvent_Deactivate_Implementation(const UCinematicContextHandle* CinematicContextHandle)
{
	// no default action here - let the actor override it
	return ICinematicHandlerInterface::Deactivate(GetOwner(), CinematicContextHandle);
}

bool UCinematicHandlerComponent::HandleCinematicEvent_MoveTo_Implementation(const UCinematicContextHandle* CinematicContextHandle, const FVector& InMoveToLocation)
{
	// let parent override - if handled, do nothing
	if (MoveTo(GetOwner(), CinematicContextHandle, InMoveToLocation))
	{
		return true;
	}
	CinematicContextHandle->AddRestoreLambda(GetOwner(), GetOwner()->GetActorLocation(), [](AActor* Actor, const FVector& InValue) { Actor->SetActorLocation(InValue, false); });
	GetOwner()->SetActorLocation(InMoveToLocation, false);
	return true;
}

bool UCinematicHandlerComponent::HandleCinematicEvent_OnBegin_Implementation(const UCinematicContextHandle* CinematicContextHandle)
{
	HandleVisibilityChange(CinematicContextHandle);
	HandleTickChange(CinematicContextHandle);
	HandleTransformChange(CinematicContextHandle);
	HandleStateChange(CinematicContextHandle);
	OnBegin(GetOwner(), CinematicContextHandle);
	return true;
}

bool UCinematicHandlerComponent::HandleCinematicEvent_OnEnd_Implementation(const UCinematicContextHandle* CinematicContextHandle)
{
	return OnEnd(GetOwner(), CinematicContextHandle);
}

bool UCinematicHandlerComponent::HandleCinematicEvent_StartTick_Implementation(const UCinematicContextHandle* CinematicContextHandle)
{
	// let the parent override this
	if (StartTick(GetOwner(), CinematicContextHandle))
	{
		return true;
	}
	CinematicContextHandle->AddRestoreLambda(GetOwner(), GetOwner()->IsActorTickEnabled(), [](AActor* Actor, const bool& bInValue) { Actor->SetActorTickEnabled(bInValue); });
	GetOwner()->SetActorTickEnabled(true);
	return true;
}

bool UCinematicHandlerComponent::HandleCinematicEvent_StopTick_Implementation(const UCinematicContextHandle* CinematicContextHandle)
{
	// let the parent override this
	if (StopTick(GetOwner(), CinematicContextHandle))
	{
		return true;
	}
	CinematicContextHandle->AddRestoreLambda(GetOwner(), GetOwner()->IsActorTickEnabled(), [](AActor* Actor, const bool& bInValue) { Actor->SetActorTickEnabled(bInValue); });
	GetOwner()->SetActorTickEnabled(false);
	return true;
}

bool UCinematicHandlerComponent::HandleCinematicEvent_Show_Implementation(const UCinematicContextHandle* CinematicContextHandle)
{
	if (Show(GetOwner(), CinematicContextHandle))
	{
		return true;
	}
	CinematicContextHandle->AddRestoreLambda(GetOwner(), GetOwner()->IsHidden(), [](AActor* Actor, const bool& bInValue) { Actor->SetActorHiddenInGame(bInValue); });
	GetOwner()->SetActorHiddenInGame(false);
	return true;
}

bool UCinematicHandlerComponent::HandleCinematicEvent_Hide_Implementation(const UCinematicContextHandle* CinematicContextHandle)
{
	if (Show(GetOwner(), CinematicContextHandle))
	{
		return true;
	}
	CinematicContextHandle->AddRestoreLambda(GetOwner(), GetOwner()->IsHidden(), [](AActor* Actor, const bool& bInValue) { Actor->SetActorHiddenInGame(bInValue); });
	GetOwner()->SetActorHiddenInGame(true);
	return true;
}

bool UCinematicHandlerComponent::ShouldHandleCinematic(const FCinematicLifeCycleEventPayload& Payload) const
{
	switch (CinematicFilter)
	{
	case ECinematicFilter::None:
		return false;
	case ECinematicFilter::All:
		return true;
	case ECinematicFilter::WithTags:
		return Payload.ContextHandle->HasAnyTag(CinematicTags);
	case ECinematicFilter::IgnoreTags:
		return !Payload.ContextHandle->HasNoneTag(CinematicTags);
	default:
		return false;
	}
}

void UCinematicHandlerComponent::OnCinematicBegin(const FCinematicLifeCycleEventPayload& Payload)
{
	if (ShouldHandleCinematic(Payload))
	{
		OnBegin(this, Payload.ContextHandle);
	}
}

void UCinematicHandlerComponent::OnCinematicEnd(const FCinematicLifeCycleEventPayload& Payload)
{
	if (ShouldHandleCinematic(Payload))
	{
		OnEnd(this, Payload.ContextHandle);
	}
}

void UCinematicHandlerComponent::HandleVisibilityChange(const UCinematicContextHandle* CinematicContextHandle)
{
	switch (CinematicVisibilityChange)
	{
	case ECinematicVisibilityChange::Show:
		Show(this, CinematicContextHandle);
		break;
	case ECinematicVisibilityChange::Hide:
		Hide(this, CinematicContextHandle);
		break;
	default:
		// do nothing
		break;
	}
}

void UCinematicHandlerComponent::HandleTickChange(const UCinematicContextHandle* CinematicContextHandle)
{
	switch (CinematicTickChange)
	{
	case ECinematicTickChange::StartTick:
		StartTick(this, CinematicContextHandle);
		break;
	case ECinematicTickChange::StopTick:
		StopTick(this, CinematicContextHandle);
		break;
	default:
		// do nothing
		break;
	}
}

void UCinematicHandlerComponent::HandleTransformChange(const UCinematicContextHandle* CinematicContextHandle)
{
	switch (CinematicTransformChange)
	{
	case ECinematicTransformChange::Move:
		MoveTo(this, CinematicContextHandle, MoveToLocation);
		break;
	default:
		// do nothing
		break;
	}
}

void UCinematicHandlerComponent::HandleStateChange(const UCinematicContextHandle* CinematicContextHandle)
{
	switch (CinematicStateChange)
	{
	case ECinematicStateChange::Activate:
		ICinematicHandlerInterface::Activate(this, CinematicContextHandle);
		break;
	case ECinematicStateChange::Deactivate:
		ICinematicHandlerInterface::Deactivate(this, CinematicContextHandle);
		break;
	default:
		break;
	}
}
