#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "TeamAffiliation.generated.h"

UENUM(BlueprintType)
enum class ETeamAffiliation : uint8
{
	None,
	Enemy,
	Player,
	Count
};


inline ETeamAttitude::Type ElectricCastleTeamAttitudeSolver(FGenericTeamId A, FGenericTeamId B)
{
	const ETeamAffiliation ATeamAffiliation = A.GetId() < static_cast<uint8>(ETeamAffiliation::Count) ? static_cast<ETeamAffiliation>(A.GetId()) : ETeamAffiliation::None;
	const ETeamAffiliation BTeamAffiliation = B.GetId() < static_cast<uint8>(ETeamAffiliation::Count) ? static_cast<ETeamAffiliation>(B.GetId()) : ETeamAffiliation::None;
	if (ATeamAffiliation == ETeamAffiliation::None || BTeamAffiliation == ETeamAffiliation::None)
	{
		return ETeamAttitude::Neutral;
	}
	return ATeamAffiliation != BTeamAffiliation ? ETeamAttitude::Hostile : ETeamAttitude::Friendly;
}
