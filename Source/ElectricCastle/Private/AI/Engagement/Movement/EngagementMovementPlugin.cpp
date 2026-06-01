// Copyright Alien Shores


#include "AI/Engagement/Movement/EngagementMovementPlugin.h"

#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Utils/RandUtils.h"


void UEngagementMovementPlugin::InitializeDependencies_Implementation(AActor* InOwnerActor)
{
	OwnerActor = InOwnerActor;
	if (const APawn* Pawn = Cast<APawn>(InOwnerActor))
	{
		MovementComponent = Pawn->GetMovementComponent();
	}
}

void UEngagementMovementPlugin::SetTargetActor_Implementation(AActor* InTargetActor)
{
	TargetActor = InTargetActor;
}

void UEngagementMovementPlugin::TickMovement_Implementation(const float DeltaTime)
{
	// Do nothing by default
}

void UEngagementMovementPlugin::ChangeItUp_Implementation()
{
	PreferredDistance = URandRangeBlueprintLibrary::GetRandomFloatInRange(PreferredDistanceRange);
	if (bDebug)
	{
		UE_LOG(LogElectricCastle, Display, TEXT("[%s:%s] Changing preferred distance: %f"), *OwnerActor->GetName(), *GetName(), PreferredDistance);
	}
}

void UEngagementMovementPlugin::SetPreferredDistance_Implementation(const FFloatRange InPreferredDistanceRange)
{
	PreferredDistanceRange = InPreferredDistanceRange;
	PreferredDistance = URandRangeBlueprintLibrary::GetRandomFloatInRange(PreferredDistanceRange);
	if (bDebug)
	{
		UE_LOG(LogElectricCastle, Display, TEXT("[%s:%s] Changing preferred distance: %f"), *OwnerActor->GetName(), *GetName(), PreferredDistance);
	}
}

UPawnMovementComponent* UEngagementMovementPlugin::GetMovementComponent()
{
	if (!IsValid(OwnerActor))
	{
		return nullptr;
	}
	if (!IsValid(MovementComponent))
	{
		if (const APawn* Pawn = Cast<APawn>(OwnerActor))
		{
			MovementComponent = Pawn->GetMovementComponent();
		}
	}
	return MovementComponent;
}

bool UEngagementMovementPlugin::GetDirectionToTarget(FVector& OutDirection) const
{
	if (!IsValid(OwnerActor))
	{
		return false;
	}
	if (!TargetActor.IsValid())
	{
		return false;
	}
	OutDirection = OwnerActor->GetActorLocation() - TargetActor->GetActorLocation();
	return true;
}
