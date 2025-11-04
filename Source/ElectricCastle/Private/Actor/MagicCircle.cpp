// Copyright Alien Shores


#include "Actor/MagicCircle.h"

#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Components/DecalComponent.h"


AMagicCircle::AMagicCircle()
{
	PrimaryActorTick.bCanEverTick = true;
	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("Magic Circle Decal"));
	DecalComponent->SetupAttachment(GetRootComponent());
}

void AMagicCircle::BeginPlay()
{
	Super::BeginPlay();
}

void AMagicCircle::SetDecalMaterial(UMaterialInterface* DecalMaterial) const
{
	if (DecalMaterial)
	{
		DecalComponent->SetDecalMaterial(DecalMaterial);
	}
}

void AMagicCircle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bRotateOnTick)
	{
		const float RotationAngle = TickRotation * DeltaTime; // Rotate 45 degrees per second
		// Create the rotation quaternion
		const FQuat RotationQuat = FQuat(RotateAxis, FMath::DegreesToRadians(RotationAngle));
		// Apply the rotation to the decal component
		DecalComponent->AddLocalRotation(RotationQuat);
	}
}
