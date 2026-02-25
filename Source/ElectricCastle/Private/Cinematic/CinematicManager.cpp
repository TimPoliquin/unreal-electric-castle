// Copyright Alien Shores


#include "Cinematic/CinematicManager.h"

#include "LevelSequencePlayer.h"
#include "Cinematic/Context/CinematicContext.h"
#include "Cinematic/Context/CinematicContextHandle.h"
#include "Cinematic/Metadata/CinematicSequenceMetaData.h"
#include "Kismet/GameplayStatics.h"

UCinematicManager* UCinematicManager::Get(const UObject* WorldContextObject)
{
	const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	return GameInstance ? GameInstance->GetSubsystem<UCinematicManager>() : nullptr;
}

void UCinematicManager::RegisterSequencePlayer(ULevelSequencePlayer* LevelSequencePlayer, ULevelSequence* LevelSequence)
{
	if (!IsValid(LevelSequencePlayer) || !IsValid(LevelSequence))
	{
		return;
	}
	CinematicContextMap.Add(LevelSequencePlayer, CreateCinematicContext(LevelSequencePlayer, LevelSequence));
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
	// cleanup
	CinematicContextMap.Remove(EventPayload.CinematicContext->GetLevelSequencePlayer());
}

UCinematicContext* UCinematicManager::CreateCinematicContext(ULevelSequencePlayer* LevelSequencePlayer, ULevelSequence* LevelSequence)
{
	UCinematicContext* Context = NewObject<UCinematicContext>(this, UCinematicContext::StaticClass());
	Context->SetLevelSequencePlayer(LevelSequencePlayer);
	Context->SetLevelSequence(LevelSequence);
	if (const UCinematicSequenceMetaData* Metadata = LevelSequence->FindMetaData<UCinematicSequenceMetaData>())
	{
		Context->SetCinematicTags(Metadata->TypeTags);
	}
	Context->OnCinematicBegin.AddDynamic(this, &UCinematicManager::HandleCinematicBegin);
	Context->OnCinematicEnd.AddDynamic(this, &UCinematicManager::HandleCinematicEnd);
	return Context;
}
