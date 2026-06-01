#pragma once

#include "CoreMinimal.h"

namespace ElectricCastleBlackboardKeys::Alert
{
	const FName AlertLevel = TEXT("AlertLevel");
	const FName AlertTargetPerceived = TEXT("AlertTargetPerceived");
	const FName LastKnownLocation = TEXT("LastKnownLocation");
}

namespace ElectricCastleBlackboardKeys::Effect
{
	const FName EffectBlockAbilities = TEXT("EffectBlockAbilities");
	const FName EffectBlockAI = TEXT("EffectBlockAI");
	const FName EffectBlockMovement = TEXT("EffectBlockMovement");
}

namespace ElectricCastleBlackboardKeys::Engagement
{
	const FName EngagementControlMode = TEXT("EngagementControlMode");
	const FName EngagementLevel = TEXT("EngagementLevel");
	const FName EngagementAttackRate = TEXT("AttackRate");
	const FName EngagementRange = TEXT("EngagementRange");
	const FName EngagementPreferredDistance = TEXT("EngagementPreferredDistance");
	const FName EngagementTarget = TEXT("EngagementTarget");
}

namespace ElectricCastleBlackboardKeys::Patrol
{
	const FName CanPatrol = TEXT("CanPatrol");
	const FName NextPatrolPoint = TEXT("NextPatrolPoint");
	const FName PatrolWaitTime = TEXT("PatrolWaitTime");
	const FName PatrolMovementSpeed = TEXT("PatrolMovementSpeed");
}

namespace ElectricCastleBlackboardKeys::Status
{
	const FName IsDead = TEXT("IsDead");
}

namespace ElectricCastleBlackboardKeys::Significance
{
	const FName SignificanceLevel = TEXT("SignificanceLevel");
}
