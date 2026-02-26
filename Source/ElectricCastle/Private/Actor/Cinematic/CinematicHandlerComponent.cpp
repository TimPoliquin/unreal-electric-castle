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
	}
	else
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[%s] No CinematicManager found!"), *GetName())
	}
	for (UCinematicEventAction* Action : CinematicActions)
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
	}
	Super::BeginDestroy();
}

void UCinematicHandlerComponent::AddCinematicAction(UCinematicEventAction* Action)
{
	if (IsValid(Action))
	{
		Action->Initialize(GetOwner());
		CinematicActions.Add(Action);
	}
}

UCinematicHandlerComponent* UCinematicHandlerComponent::GetCinematicHandlerComponent_Implementation()
{
	return this;
}

void UCinematicHandlerComponent::OnCinematicBegin(const FCinematicLifeCycleEventPayload& Payload)
{
	if (bDebug)
	{
		UE_LOG(LogElectricCastle, Log, TEXT("[%s:%s] OnCinematicBegin - Executing %d Actions"), *GetOwner()->GetName(), *GetName(), CinematicActions.Num())
	}
	for (UCinematicEventAction* Action : CinematicActions)
	{
		if (!IsValid(Action))
		{
			return;
		}
		Action->SetDebugEnabled(bDebug || Action->IsDebugEnabled());
		if (Action->ShouldExecute(Payload.ContextHandle))
		{
			if (bDebug)
			{
				UE_LOG(LogElectricCastle, Log, TEXT("[%s] Executing action %s"), *GetName(), *Action->GetClass()->GetName())
			}
			Action->Execute(Payload.ContextHandle);
		}
	}
}
