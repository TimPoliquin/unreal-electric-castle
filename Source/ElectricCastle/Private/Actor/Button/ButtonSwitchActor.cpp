// Copyright Alien Shores


#include "Actor/Button/ButtonSwitchActor.h"

#include "Components/BoxComponent.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"


AButtonSwitchActor::AButtonSwitchActor()
{
	PrimaryActorTick.bCanEverTick = false;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	BaseMesh->SetupAttachment(GetRootComponent());
	InitializeMeshCollisionResponse(BaseMesh);
	SwitchMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Switch Mesh"));
	SwitchMesh->SetupAttachment(GetRootComponent());
	InitializeMeshCollisionResponse(SwitchMesh);
	SwitchDetector = CreateDefaultSubobject<UBoxComponent>(TEXT("Switch Detector"));
	SwitchDetector->SetupAttachment(GetRootComponent());
	SwitchDetector->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SwitchDetector->SetCollisionObjectType(ECC_WorldDynamic);
	SwitchDetector->SetCollisionResponseToAllChannels(ECR_Ignore);
	SwitchDetector->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	SwitchDetector->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	SwitchDetector->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AButtonSwitchActor::ResetSwitch_Implementation()
{
	if (!bIsActivated)
	{
		return;
	}
	if (CanDeactivateSwitch(SwitchDetector))
	{
		DeactivateSwitch();
	}
	else
	{
		UE_LOG(LogElectricCastle, Log, TEXT("[%s] Attempted to ResetSwitch, but was not able to due to active collisions!"), *GetName())
	}
}

void AButtonSwitchActor::BeginPlay()
{
	Super::BeginPlay();
	SwitchDetector->OnComponentBeginOverlap.AddDynamic(this, &AButtonSwitchActor::OnSwitchDetectorBeginOverlap);
	SwitchDetector->OnComponentEndOverlap.AddDynamic(this, &AButtonSwitchActor::OnSwitchDetectorEndOverlap);
}

bool AButtonSwitchActor::ActivateSwitch_Implementation()
{
	if (bIsActivated)
	{
		return false;
	}
	bIsActivated = true;
	UpdateSwitchMesh_Activate(SwitchMesh);
	OnActivatedDelegate.Broadcast(FOnSwitchStatusChangedPayload(this));
	return true;
}

bool AButtonSwitchActor::DeactivateSwitch_Implementation()
{
	if (!bIsActivated || bIsActivationLocked)
	{
		return false;
	}
	bIsActivated = false;
	UpdateSwitchMesh_Deactivate(SwitchMesh);
	OnDeactivatedDelegate.Broadcast(FOnSwitchStatusChangedPayload(this));
	return true;
}

void AButtonSwitchActor::UpdateSwitchMesh_Activate_Implementation(UStaticMeshComponent* InSwitchMesh)
{
	InSwitchMesh->AddWorldOffset(FVector::DownVector * 3);
}

void AButtonSwitchActor::UpdateSwitchMesh_Deactivate_Implementation(UStaticMeshComponent* InSwitchMesh)
{
	InSwitchMesh->AddWorldOffset(FVector::UpVector * 3);
}

void AButtonSwitchActor::OnSwitchDetectorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                                      const FHitResult& SweepResult)
{
	ActivateSwitch();
}

void AButtonSwitchActor::OnSwitchDetectorEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!bIsActivated)
	{
		return;
	}
	if (CanDeactivateSwitch(OverlappedComponent))
	{
		DeactivateSwitch();
	}
}

bool AButtonSwitchActor::CanDeactivateSwitch(const UPrimitiveComponent* DetectionComponent) const
{
	TArray<AActor*> OverlappingActors;
	DetectionComponent->GetOverlappingActors(OverlappingActors);
	OverlappingActors.RemoveAll([this](const AActor* Actor) { return Actor == this; });
	return OverlappingActors.Num() <= 0;
}

void AButtonSwitchActor::InitializeMeshCollisionResponse(UStaticMeshComponent* MeshComponent)
{
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionObjectType(ECC_WorldStatic);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	MeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	MeshComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	MeshComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	MeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
}
