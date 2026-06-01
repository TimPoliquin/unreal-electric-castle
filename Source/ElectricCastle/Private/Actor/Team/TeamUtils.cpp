// Copyright Alien Shores


#include "Actor/Team/TeamUtils.h"

#include "AI/Affiliation/TeamAffiliation.h"

FGenericTeamId UTeamUtils::GetTeamId(const AActor* Actor)
{
	if (const IGenericTeamAgentInterface* TeamAgentInterface = Cast<IGenericTeamAgentInterface>(Actor))
	{
		return TeamAgentInterface->GetGenericTeamId();
	}
	return FGenericTeamId::NoTeam;
}

ETeamAttitude::Type UTeamUtils::GetTeamAttitude(const AActor* Source, const AActor* Target)
{
	if (!IsValid(Source) || !IsValid(Target))
	{
		return ETeamAttitude::Neutral;
	}
	if (const IGenericTeamAgentInterface* TargetTeamInterface = Cast<IGenericTeamAgentInterface>(Target))
	{
		return TargetTeamInterface->GetTeamAttitudeTowards(*Source);
	}
	return ETeamAttitude::Neutral;
}

bool UTeamUtils::HaveHostileRelationship(const AActor* Source, const AActor* Target)
{
	return GetTeamAttitude(Source, Target) == ETeamAttitude::Hostile;
}
