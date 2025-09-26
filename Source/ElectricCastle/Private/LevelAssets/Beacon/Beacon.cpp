// Copyright Alien Shores


#include "LevelAssets/Beacon/Beacon.h"

#include "ElectricCastle/ElectricCastle.h"


// Sets default values
ABeacon::ABeacon()
{
	PrimaryActorTick.bCanEverTick = false;
	BeaconMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BeaconMesh"));
	BeaconMesh->SetupAttachment(GetRootComponent());
	BeaconMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BeaconMesh->SetCollisionResponseToAllChannels(ECR_Block);
	BeaconMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
	BeaconMesh->SetRenderCustomDepth(false);
	BeaconMesh->MarkRenderStateDirty();
}

void ABeacon::BeginPlay()
{
	Super::BeginPlay();
	OriginalMaterial = BeaconMesh->GetMaterial(0);
	DynamicMaterialInstance = UMaterialInstanceDynamic::Create(
		OriginalMaterial,
		this
	);
	if (bIsSwitchActive)
	{
		ActivateBeacon(true);
	}
}

void ABeacon::HighlightActor_Implementation()
{
	if (IsPOIDisabled())
	{
		return;
	}
	BeaconMesh->SetRenderCustomDepth(true);
}

void ABeacon::UnHighlightActor_Implementation()
{
	BeaconMesh->SetRenderCustomDepth(false);
}

void ABeacon::PostLoad_Implementation()
{
	Super::PostLoad_Implementation();
	if (bIsSwitchActive)
	{
		ActivateBeacon(true);
	}
}

void ABeacon::BroadcastOnBeaconActivated()
{
	OnBeaconActivated.Broadcast(FOnSwitchStatusChangedPayload(this));
}

void ABeacon::BroadcastOnBeaconReset()
{
	OnBeaconReset.Broadcast(FOnSwitchStatusChangedPayload(this));
}

void ABeacon::HandleInteract_Implementation(AActor* Player)
{
	ActivateBeacon(false);
}

void ABeacon::ActivateBeacon_Implementation(const bool bForced)
{
	if (bIsSwitchActive)
	{
		return;
	}
	bIsSwitchActive = true;
	BeaconMesh->SetMaterial(0, DynamicMaterialInstance);
	DisablePOI();
}

void ABeacon::ResetSwitch_Implementation()
{
	BeaconMesh->SetMaterial(0, OriginalMaterial);
	bIsSwitchActive = false;
	EnablePOI();
}


EBeaconValidationState ABeacon::ValidateBeaconActivation(const AActor* BeaconActor, const TArray<ABeacon*>& Beacons, const bool IsOrdered)
{
	EBeaconValidationState ValidationState = EBeaconValidationState::Complete;
	bool CurrentBeaconFound = false;
	for (const ABeacon* Beacon : Beacons)
	{
		if (Beacon == BeaconActor)
		{
			CurrentBeaconFound = true;
		}
		if (!IsSwitchActive(Beacon))
		{
			if (IsOrdered && !CurrentBeaconFound)
			{
				ValidationState = EBeaconValidationState::OutOfOrder;
			}
			else
			{
				ValidationState = EBeaconValidationState::Incomplete;
			}
			break;
		}
	}
	return ValidationState;
}
