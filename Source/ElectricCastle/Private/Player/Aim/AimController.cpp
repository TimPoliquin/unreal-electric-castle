// Copyright Alien Shores


#include "Player/Aim/AimController.h"

#include "KismetTraceUtils.h"
#include "AbilitySystem/ElectricCastleAbilitySystemComponent.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "Actor/Highlight/HighlightActorInterface.h"
#include "ElectricCastle/ElectricCastle.h"
#include "Kismet/KismetMathLibrary.h"
#include "Tags/ElectricCastleGameplayTags.h"


// Sets default values for this component's properties
UAimController::UAimController()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAimController::HandleAbilitySystemReady(UElectricCastleAbilitySystemComponent* InAbilitySystemComponent)
{
	const FElectricCastleGameplayTags& GameplayTags = FElectricCastleGameplayTags::Get();
	InAbilitySystemComponent->RegisterGameplayTagEvent(GameplayTags.Player_Block_Aim, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UAimController::HandleTagChange_BlockAim);
	HandleTagChange_BlockAim(GameplayTags.Player_Block_Aim, InAbilitySystemComponent->GetTagCount(GameplayTags.Player_Block_Aim));
}

void UAimController::AimStart()
{
	if (bCanAim)
	{
		SetIsAiming(true);
	}
}

void UAimController::AimEnd()
{
	SetIsAiming(false);
}

bool UAimController::GetCanAim() const
{
	return bCanAim;
}

bool UAimController::IsAiming() const
{
	return bIsAiming;
}

bool UAimController::HasTarget() const
{
	return TraceResult.bBlockingHit && IHighlightActorInterface::IsHighlightActor(TraceResult.GetActor());
}

AActor* UAimController::GetTarget() const
{
	return TraceResult.GetActor();
}

FVector UAimController::GetHitLocation() const
{
	return TraceResult.bBlockingHit ? TraceResult.ImpactPoint : TraceEndLocation;
}

FRotator UAimController::CalculateRotationToFaceAimTarget(const FVector& Location) const
{
	return UKismetMathLibrary::FindLookAtRotation(Location, GetHitLocation());
}

void UAimController::TraceForTarget(const FVector& PlayerLocation, const FVector& CameraLocation, const FRotator& CameraRotation)
{
	if (!bCanAim)
	{
		return;
	}
	TraceEndLocation = CameraLocation + (CameraRotation.Vector() * TraceDistance);
	const FVector TraceStartLocation = CalculateTraceStartLocation(PlayerLocation, CameraLocation, TraceEndLocation);


	// Setup collision query parameters
	FHitResult HitResult;
	FCollisionQueryParams TraceParams(FName(TEXT("CameraTrace")));
	TraceParams.AddIgnoredActor(GetOwner());
	TraceParams.bTraceComplex = false;
	TraceParams.bReturnPhysicalMaterial = false;

	GetWorld()->SweepSingleByChannel(
		HitResult,
		TraceStartLocation,
		TraceEndLocation,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(TraceRadius),
		TraceParams
	);

	if (bDebug)
	{
		DrawDebugSphereTraceSingle(GetWorld(), CameraLocation, TraceEndLocation, TraceRadius, EDrawDebugTrace::ForOneFrame, true, HitResult, FColor::Green, FColor::Red, 0.f);
	}
	if (HitResult.GetActor() != TraceResult.GetActor())
	{
		SetTarget(HitResult.GetActor());
	}
	TraceResult = HitResult;
}

FVector UAimController::CalculateTraceStartLocation(const FVector& PlayerLocation, const FVector& CameraLocation, const FVector& TraceEnd)
{
	const FVector AB = TraceEnd - CameraLocation;
	const float ABLengthSquared = AB.SizeSquared();

	// Handle degenerate case where A and B are the same
	if (ABLengthSquared <= KINDA_SMALL_NUMBER)
	{
		return CameraLocation;
	}

	// Project AP onto AB, normalized by AB length squared
	float T = FVector::DotProduct(PlayerLocation - CameraLocation, AB) / ABLengthSquared;

	// Clamp T to segment range [0, 1]
	T = FMath::Clamp(T, 0.0f, 1.0f);

	// Return the closest point
	return CameraLocation + T * AB;
}

void UAimController::HandleTagChange_BlockAim(FGameplayTag BlockAimTag, const int Count)
{
	SetCanAim(Count == 0);
}

void UAimController::SetCanAim(const bool bInCanAim)
{
	bCanAim = bInCanAim;
	if (bCanAim)
	{
		OnCanAim.Broadcast();
	}
	else
	{
		OnCannotAim.Broadcast();
	}
}

void UAimController::SetIsAiming(const bool bInIsAiming)
{
	if (bIsAiming == bInIsAiming)
	{
		return;
	}
	bIsAiming = bInIsAiming;
	if (bIsAiming)
	{
		AimingEffectHandle = UElectricCastleAbilitySystemLibrary::ApplyInfiniteEffectByTag(GetOwner(), FElectricCastleGameplayTags::Get().Effect_State_Aiming);
		OnAimStart.Broadcast();
		return;
	}
	if (AimingEffectHandle.IsValid())
	{
		UElectricCastleAbilitySystemLibrary::RemoveGameplayEffect(GetOwner(), AimingEffectHandle);
	}
	OnAimEnd.Broadcast();
}

void UAimController::SetTarget(AActor* InTarget) const
{
	if (IHighlightActorInterface::IsHighlightActor(InTarget))
	{
		IHighlightActorInterface::Highlight(InTarget);
		OnTargetChanged.Broadcast(FTargetChangedPayload(InTarget));
	}
	else
	{
		ClearTarget(TraceResult.GetActor());
	}
}

void UAimController::ClearTarget(const AActor* InOldTarget) const
{
	if (IsValid(InOldTarget))
	{
		IHighlightActorInterface::Unhighlight(InOldTarget);
		OnTargetChanged.Broadcast(FTargetChangedPayload(nullptr));
	}
}
