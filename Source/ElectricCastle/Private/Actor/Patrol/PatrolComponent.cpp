// Copyright Alien Shores


#include "Actor/Patrol/PatrolComponent.h"

#include "NavigationSystem.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"


UPatrolComponent::UPatrolComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UPatrolComponent::CanPatrol() const
{
	return PatrolType > EPatrolType::None && PatrolType < EPatrolType::Count;
}

FVector UPatrolComponent::GetNextPatrolLocation()
{
	switch (PatrolType)
	{
	case EPatrolType::Predefined:
		if (PatrolTargets.IsEmpty())
		{
			UE_LOG(LogElectricCastle, Error, TEXT("[%s:%s] No patrol targets defined!"), *GetOwner()->GetName(), *GetName());
			return GetOwner()->GetActorLocation();
		}
		if (bPatrolTargetsInOrder)
		{
			PatrolTargetIndex = (PatrolTargetIndex + 1) % PatrolTargets.Num();
			return GetPatrolLocation(PatrolTargetIndex);
		}
		return GetPatrolLocation(FMath::RandRange(0, PatrolTargets.Num() - 1));
	case EPatrolType::Random:
		return GetRandomPatrolLocation();
	default:
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] Invalid patrol type: %s"), *GetOwner()->GetName(), *GetName(), *UEnum::GetValueAsString(PatrolType));
		return GetOwner()->GetActorLocation();
	}
}

float UPatrolComponent::GetPatrolMovementSpeed() const
{
	return URandRangeBlueprintLibrary::GetRandomFloatInRange(PatrolSpeed);
}

float UPatrolComponent::GetPatrolWaitTime() const
{
	return URandRangeBlueprintLibrary::GetRandomFloatInRange(PatrolWaitTime);
}

FVector UPatrolComponent::GetRandomPatrolLocation() const
{
	FVector NextPatrolLocation;
	UNavigationSystemV1::K2_GetRandomReachablePointInRadius(GetOwner(), GetOwner()->GetActorLocation(), NextPatrolLocation, PatrolRadius);
	return NextPatrolLocation;
}

FVector UPatrolComponent::GetPatrolLocation(const uint32 InPatrolTargetIndex) const
{
	if (const AActor* PatrolTarget = PatrolTargets.IsValidIndex(InPatrolTargetIndex) ? PatrolTargets[InPatrolTargetIndex] : nullptr)
	{
		return PatrolTarget->GetActorLocation();
	}
	UE_LOG(LogElectricCastle, Error, TEXT("[%s:%s] Invalid patrol index: %d. PatrolTargets: %d"), *GetOwner()->GetName(), *GetName(), InPatrolTargetIndex, PatrolTargets.Num());
	return GetOwner()->GetActorLocation();
}
