// Copyright Alien Shores


#include "Cinematic/Context/CinematicContext.h"

#include "LevelSequencePlayer.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "Actor/Cinematic/CinematicRelocationTarget.h"
#include "Cinematic/Metadata/CinematicSequenceMetaData.h"
#include "Kismet/GameplayStatics.h"

ULevelSequencePlayer* UCinematicContext::GetLevelSequencePlayer() const
{
	return LevelSequencePlayer.Get();
}

ULevelSequence* UCinematicContext::GetLevelSequence() const
{
	return LevelSequence.Get();
}

bool UCinematicContext::HasTag(const FGameplayTag& Tag) const
{
	if (!Metadata.IsValid())
	{
		return false;
	}
	return Metadata->HasTypeTag(Tag) || Metadata->HasReactionTriggerTag(Tag);
}

bool UCinematicContext::HasAnyTag(const FGameplayTagContainer& Tags) const
{
	if (!Metadata.IsValid())
	{
		return false;
	}
	return Metadata->HasAnyTypeTags(Tags) || Metadata->HasAnyReactionTriggerTags(Tags);
}

bool UCinematicContext::ShouldRelocatePlayer() const
{
	if (!Metadata.IsValid())
	{
		return false;
	}
	return Metadata->GetPlayerRelocationMode() == ECinematicPlayerRelocationMode::Level || Metadata->GetPlayerRelocationMode() == ECinematicPlayerRelocationMode::CinematicOverride;
}

FVector UCinematicContext::GetPlayerRelocationLocation() const
{
	if (!Metadata.IsValid())
	{
		return FVector::ZeroVector;
	}
	if (Metadata->GetPlayerRelocationMode() == ECinematicPlayerRelocationMode::CinematicOverride)
	{
		return Metadata->GetPlayerRelocationLocation();
	}
	if (LevelSequencePlayer.IsValid())
	{
		TArray<AActor*> LevelRelocationTargets;
		UGameplayStatics::GetAllActorsOfClass(LevelSequencePlayer.Get(), ACinematicRelocationTarget::StaticClass(), LevelRelocationTargets);
		if (LevelRelocationTargets.Num() > 0)
		{
			return LevelRelocationTargets[0]->GetActorLocation();
		}
	}
	return FVector::ZeroVector;
}

void UCinematicContext::RestoreAll()
{
	RestoreStack.RestoreAll();
}

void UCinematicContext::SetLevelSequencePlayer(ULevelSequencePlayer* InPlayer)
{
	LevelSequencePlayer = InPlayer;
	if (!IsValid(InPlayer))
	{
		return;
	}
	InPlayer->OnPlay.AddDynamic(this, &UCinematicContext::HandleOnPlay);
	InPlayer->OnFinished.AddDynamic(this, &UCinematicContext::HandleOnFinished);
	InPlayer->OnStop.AddDynamic(this, &UCinematicContext::HandleOnFinished);
}

void UCinematicContext::SetLevelSequence(ULevelSequence* InSequence)
{
	LevelSequence = InSequence;
	Metadata = LevelSequence.IsValid() ? LevelSequence->FindMetaData<UCinematicSequenceMetaData>() : nullptr;
}

void UCinematicContext::AddRestoreFunction(const TFunction<void()>& InRestoreFunc)
{
	RestoreStack.Add(InRestoreFunc);
}

void UCinematicContext::HandleOnPlay()
{
	OnCinematicBegin.Broadcast(FCinematicContextEventPayload(ECinematicLifeCycle::Begin, this));
}

void UCinematicContext::HandleOnFinished()
{
	RestoreStack.RestoreAll();
	UnbindDelegates();
	OnCinematicEnd.Broadcast(FCinematicContextEventPayload(ECinematicLifeCycle::End, this));
}

void UCinematicContext::UnbindDelegates()
{
	if (LevelSequencePlayer.IsValid())
	{
		LevelSequencePlayer->OnPlay.RemoveDynamic(this, &UCinematicContext::HandleOnPlay);
		LevelSequencePlayer->OnFinished.RemoveDynamic(this, &UCinematicContext::HandleOnFinished);
	}
}
