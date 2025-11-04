// Copyright Alien Shores


#include "Actor/FadeActor.h"

#include "ElectricCastle/ElectricCastle.h"
#include "Components/TimelineComponent.h"
#include "Kismet/KismetMaterialLibrary.h"


// Sets default values
AFadeActor::AFadeActor()
{
	PrimaryActorTick.bCanEverTick = false;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(MeshComponent);
	MeshComponent->SetCollisionObjectType(ECC_WorldStatic);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Block);
	MeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	MeshComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	TimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("Timeline"));
	OnTimelineTickDelegate.BindDynamic(this, &AFadeActor::OnTimelineTick);
	OnFadeOutFinishedDelegate.BindDynamic(this, &AFadeActor::OnFadeOutComplete);
	OnFadeInFinishedDelegate.BindDynamic(this, &AFadeActor::OnFadeInComplete);
}

void AFadeActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	OriginalVisibilityResponse = MeshComponent->GetCollisionResponseToChannel(ECC_Visibility);
	OriginalMaterials = MeshComponent->GetMaterials();
	DynamicMaterials.Empty();
	for (UMaterialInstance* FadeMaterialDef : FadeMaterialInstances)
	{
		DynamicMaterials.Add(
			UKismetMaterialLibrary::CreateDynamicMaterialInstance(
				this,
				FadeMaterialDef
			)
		);
	}
}

void AFadeActor::BeginPlay()
{
	Super::BeginPlay();
	TimelineComponent->AddInterpFloat(FadeCurve, OnTimelineTickDelegate);
}

void AFadeActor::FadeIn_Implementation()
{
	IFadeInterface::FadeIn_Implementation();
	TimelineComponent->SetTimelineFinishedFunc(OnFadeInFinishedDelegate);
	TimelineComponent->Reverse();
}

void AFadeActor::FadeOut_Implementation()
{
	ReplaceMaterials(DynamicMaterials);
	MeshComponent->SetCastShadow(false);
	MeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	MeshComponent->SetCollisionResponseToChannel(ECC_Target, ECR_Ignore);
	TimelineComponent->SetTimelineFinishedFunc(OnFadeOutFinishedDelegate);
	TimelineComponent->Play();
}

void AFadeActor::OnFadeInComplete()
{
	ReplaceMaterials(OriginalMaterials);
	MeshComponent->SetCastShadow(true);
	MeshComponent->SetCollisionResponseToChannel(ECC_Visibility, OriginalVisibilityResponse);
	MeshComponent->SetCollisionResponseToChannel(ECC_Target, ECR_Block);
}

void AFadeActor::OnFadeOutComplete()
{
	// nothing to do right now
}

void AFadeActor::OnTimelineTick(const float FadeValue)
{
	for (UMaterialInterface* Material : MeshComponent->GetMaterials())
	{
		if (UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(Material))
		{
			DynamicMaterial->SetScalarParameterValue(FName("Fade"), FadeValue);
		}
	}
}
