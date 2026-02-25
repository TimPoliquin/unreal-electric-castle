// Copyright Alien Shores


#include "Actor/Cinematic/CinematicHandlerComponent.h"

#include "Actor/Cinematic/Actions/CinematicEventAction.h"
#include "Cinematic/CinematicManager.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"


UCinematicHandlerComponent::UCinematicHandlerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCinematicHandlerComponent::BeginPlay()
{
	Super::BeginPlay();
	if (bDebug)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s] InitializeComponent"), *GetName())
	}
	if (UCinematicManager* CinematicManager = UCinematicManager::Get(GetOwner()))
	{
		CinematicManager->OnCinematicBegin.AddUniqueDynamic(this, &UCinematicHandlerComponent::OnCinematicBegin);
		CinematicManager->OnCinematicEnd.AddUniqueDynamic(this, &UCinematicHandlerComponent::OnCinematicEnd);
	}
	else
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[%s] No CinematicManager found!"), *GetName())
	}
	for (UCinematicEventAction* Action : CinematicBeginActions)
	{
		if (IsValid(Action))
		{
			Action->Initialize(GetOwner());
		}
	}
	for (UCinematicEventAction* Action : CinematicEndActions)
	{
		if (IsValid(Action))
		{
			Action->Initialize(GetOwner());
		}
	}
}

void UCinematicHandlerComponent::BeginDestroy()
{
	if (bDebug)
	{
		UE_LOG(LogElectricCastle, Log, TEXT("[%s] BeginDestroy"), *GetName())
	}
	if (UCinematicManager* CinematicManager = UCinematicManager::Get(GetOwner()))
	{
		CinematicManager->OnCinematicBegin.RemoveDynamic(this, &UCinematicHandlerComponent::OnCinematicBegin);
		CinematicManager->OnCinematicEnd.RemoveDynamic(this, &UCinematicHandlerComponent::OnCinematicEnd);
	}
	Super::BeginDestroy();
}

UCinematicHandlerComponent* UCinematicHandlerComponent::GetCinematicHandlerComponent_Implementation()
{
	return this;
}

void UCinematicHandlerComponent::OnCinematicBegin(const FCinematicLifeCycleEventPayload& Payload)
{
	if (bDebug)
	{
		UE_LOG(LogElectricCastle, Log, TEXT("[%s:%s] OnCinematicBegin - Executing %d Actions"), *GetOwner()->GetName(), *GetName(), CinematicBeginActions.Num())
	}
	for (const UCinematicEventAction* Action : CinematicBeginActions)
	{
		if (IsValid(Action) && Action->ShouldExecute(Payload.ContextHandle))
		{
			if (bDebug)
			{
				UE_LOG(LogElectricCastle, Log, TEXT("[%s] Executing action %s"), *GetName(), *Action->GetClass()->GetName())
			}
			Action->Execute(Payload.ContextHandle);
		}
	}
}

void UCinematicHandlerComponent::OnCinematicEnd(const FCinematicLifeCycleEventPayload& Payload)
{
	if (bDebug)
	{
		UE_LOG(LogElectricCastle, Log, TEXT("[%s:%s] OnCinematicEnd - Executing %d Actions"), *GetOwner()->GetName(), *GetName(), CinematicEndActions.Num())
	}
	for (const UCinematicEventAction* Action : CinematicEndActions)
	{
		if (IsValid(Action) && Action->ShouldExecute(Payload.ContextHandle))
		{
			Action->Execute(Payload.ContextHandle);
		}
	}
}
