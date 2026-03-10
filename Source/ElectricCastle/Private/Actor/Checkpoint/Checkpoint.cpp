// Copyright Alien Shores


#include "Actor/Checkpoint/Checkpoint.h"

#include "Actor/Highlight/HighlightComponent.h"
#include "ElectricCastle/ElectricCastle.h"
#include "Components/SphereComponent.h"
#include "Game/Save/SaveGameManager.h"
#include "Interaction/PlayerInterface.h"

ACheckpoint::ACheckpoint(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	CheckpointMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CheckpointMesh"));
	CheckpointMesh->SetupAttachment(GetRootComponent());
	CheckpointMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CheckpointMesh->SetCollisionResponseToAllChannels(ECR_Block);
	CheckpointMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_TAN);
	CheckpointMesh->MarkRenderStateDirty();
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(CheckpointMesh);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	MoveToComponent = CreateDefaultSubobject<USceneComponent>(TEXT("MoveToComponent"));
	MoveToComponent->SetupAttachment(GetRootComponent());
	HighlightComponent = CreateDefaultSubobject<UHighlightComponent>(TEXT("HighlightComponent"));
	HighlightComponent->SetHighlightType(EHighlightType::Friendly);
}

void ACheckpoint::PostLoad_Implementation()
{
	if (bHasBeenActivated)
	{
		PlayActivatedEffect();
	}
}

void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::OnSphereOverlap);
	Execute_PostLoad(this);
}

void ACheckpoint::OnSphereOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (IsValid(OtherActor) && OtherActor->Implements<UPlayerInterface>())
	{
		bHasBeenActivated = true;
		PlayActivatedEffect();
		if (USaveGameManager* SaveGameManager = USaveGameManager::Get(this))
		{
			FAuraSaveGameParams SaveGameParams;
			SaveGameParams.DestinationMapName = GetWorld()->GetPathName();
			SaveGameParams.DestinationPlayerStartTag = PlayerStartTag;
			SaveGameManager->SaveGame(SaveGameParams);
		}
	}
}

UHighlightComponent* ACheckpoint::GetHighlightComponent_Implementation() const
{
	return HighlightComponent;
}

void ACheckpoint::GetHighlightMeshes_Implementation(TArray<UMeshComponent*>& OutHighlightMeshes)
{
	OutHighlightMeshes.Add(CheckpointMesh);
}

void ACheckpoint::PlayActivatedEffect_Implementation()
{
	if (bDisableAfterActivation)
	{
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	UMaterialInstanceDynamic* DynamicMaterialInstance = UMaterialInstanceDynamic::Create(
		CheckpointMesh->GetMaterial(0),
		this
	);
	CheckpointMesh->SetMaterial(0, DynamicMaterialInstance);
	CheckpointReached(DynamicMaterialInstance);
}
