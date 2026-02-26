// Copyright Alien Shores


#include "Cinematic/Context/CinematicContext.h"

#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "Actor/Cinematic/CinematicRelocationTarget.h"
#include "Cinematic/Metadata/CinematicSequenceMetaData.h"
#include "Kismet/GameplayStatics.h"

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
	if (IsValid(GetWorld()))
	{
		TArray<AActor*> LevelRelocationTargets;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACinematicRelocationTarget::StaticClass(), LevelRelocationTargets);
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

void UCinematicContext::SetSequenceMetadata(UCinematicSequenceMetadata* InMetadata)
{
	Metadata = InMetadata;
}

void UCinematicContext::AddRestoreFunction(const TFunction<void()>& InRestoreFunc)
{
	RestoreStack.Add(InRestoreFunc);
}

UWorld* UCinematicContext::GetWorld() const
{
	return Super::GetWorld();
}
