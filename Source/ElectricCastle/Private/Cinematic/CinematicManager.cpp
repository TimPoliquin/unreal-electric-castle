// Copyright Alien Shores


#include "Cinematic/CinematicManager.h"

#include "Cinematic/Context/CinematicContext.h"
#include "Cinematic/Context/CinematicContextHandle.h"
#include "Kismet/GameplayStatics.h"

UCinematicManager* UCinematicManager::Get(const UObject* WorldContextObject)
{
	const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	return GameInstance ? GameInstance->GetSubsystem<UCinematicManager>() : nullptr;
}

void UCinematicManager::RegisterContext(UCinematicContext* InContext)
{
	if (IsValid(InContext))
	{
		InContext->OnCinematicBegin.AddDynamic(this, &UCinematicManager::HandleCinematicBegin);
		InContext->OnCinematicEnd.AddDynamic(this, &UCinematicManager::HandleCinematicEnd);
	}
}

void UCinematicManager::HandleCinematicBegin(const FCinematicContextEventPayload& EventPayload)
{
	UCinematicContextHandle* Handle = NewObject<UCinematicContextHandle>(this);
	Handle->Initialize(EventPayload.CinematicContext);
	OnCinematicBegin.Broadcast(FCinematicLifeCycleEventPayload(EventPayload.EventType, Handle));
}

void UCinematicManager::HandleCinematicEnd(const FCinematicContextEventPayload& EventPayload)
{
	// notify listeners that the cinematic has finished
	UCinematicContextHandle* Handle = NewObject<UCinematicContextHandle>(this);
	Handle->Initialize(EventPayload.CinematicContext);
	OnCinematicEnd.Broadcast(FCinematicLifeCycleEventPayload(EventPayload.EventType, Handle));
}
