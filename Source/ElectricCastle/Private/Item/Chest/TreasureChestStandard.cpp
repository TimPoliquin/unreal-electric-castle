// Copyright Alien Shores


#include "Item/Chest/TreasureChestStandard.h"

#include "NiagaraComponent.h"
#include "Actor/Effect/FadeEffectComponent.h"
#include "ElectricCastle/ElectricCastle.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PointLightComponent.h"
#include "Item/Component/LockComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ATreasureChestStandard::ATreasureChestStandard()
{
	PrimaryActorTick.bCanEverTick = true;
	OverlapDetectionComponent->SetCapsuleHalfHeight(100.f);
	OverlapDetectionComponent->SetCapsuleRadius(75.f);
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>("Arrow Component");
	ArrowComponent->SetupAttachment(RootComponent);
	ChestBaseMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Chest Base Mesh"));
	ChestBaseMeshComponent->SetupAttachment(GetRootComponent());
	LidMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lid Mesh"));
	LidMeshComponent->SetupAttachment(ChestBaseMeshComponent);
	LootMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Loot Mesh"));
	LootMeshComponent->SetupAttachment(ChestBaseMeshComponent);
	LootLightComponent = CreateDefaultSubobject<UPointLightComponent>(TEXT("Loot Light Component"));
	LootLightComponent->SetupAttachment(LootMeshComponent);
	LootLightComponent->Intensity = 10000.f;
	LootLightComponent->LightColor = FColor(232, 184, 37);
	LootLightComponent->AttenuationRadius = 1000;
	LootLightComponent->SourceRadius = 96;
	LootLightComponent->SoftSourceRadius = 96;
	LootEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Loot Effect Component"));
	LootEffectComponent->SetupAttachment(LootMeshComponent);
	LockRingComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Lock Ring Component"));
	LockRingComponent->SetupAttachment(GetRootComponent());
	BoxCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Base Collision"));
	BoxCollisionComponent->SetupAttachment(RootComponent);
	BoxCollisionComponent->SetBoxExtent(FVector(47, 87, 32));
	BoxCollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxCollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxCollisionComponent->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	BoxCollisionComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	BoxCollisionComponent->SetCollisionResponseToChannel(ECC_Target, ECR_Block);
	BoxCollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	FadeEffectComponent = CreateDefaultSubobject<UFadeEffectComponent>(TEXT("Loot Fade Effect Component"));
	OnUnlockUpdate.BindDynamic(this, &ATreasureChestStandard::UnlockEffect_Update);
	OnUnlockFinish.BindDynamic(this, &ATreasureChestStandard::UnlockEffect_Finish);
	OnOpenUpdate.BindDynamic(this, &ATreasureChestStandard::OpenEffect_Update);
	OnOpenFinish.BindDynamic(this, &ATreasureChestStandard::OpenEffect_Finish);
}

void ATreasureChestStandard::HighlightActor_Implementation()
{
	ChestBaseMeshComponent->SetRenderCustomDepth(true);
	LidMeshComponent->SetRenderCustomDepth(true);
}

void ATreasureChestStandard::UnHighlightActor_Implementation()
{
	ChestBaseMeshComponent->SetRenderCustomDepth(false);
	LidMeshComponent->SetRenderCustomDepth(false);
}

void ATreasureChestStandard::BeginPlay()
{
	Super::BeginPlay();
	InitializeHighlight();
	InitializeSwitchLockEffect();
	InitializeInteractionWidget();
	InitializeLootLightIntensity();
	InitializeLootFadeEffect();
}

void ATreasureChestStandard::InitializeLootLightIntensity_Implementation()
{
	TreasureLightIntensity = LootLightComponent->Intensity;
	LootLightComponent->SetIntensity(0.f);
}

void ATreasureChestStandard::InitializeInteractionWidget_Implementation()
{
}

void ATreasureChestStandard::InitializeSwitchLockEffect_Implementation()
{
	if (LockComponent->IsUnlocked())
	{
		SwitchLockRing_Deactivate();
	}
	else if (LockComponent->IsUnlockedBySwitch())
	{
		SwitchLockRing_Activate();
	}
	else
	{
		SwitchLockRing_Deactivate();
	}
}

void ATreasureChestStandard::InitializeHighlight_Implementation()
{
	ChestBaseMeshComponent->SetRenderCustomDepth(false);
	ChestBaseMeshComponent->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
	LidMeshComponent->SetRenderCustomDepth(false);
	LidMeshComponent->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
}

void ATreasureChestStandard::SwitchLockRing_Activate_Implementation()
{
	if (LockRingComponent)
	{
		LockRingComponent->Activate();
	}
}

void ATreasureChestStandard::SwitchLockRing_Deactivate_Implementation()
{
	if (LockRingComponent)
	{
		LockRingComponent->Deactivate();
		LockRingComponent->DestroyComponent();
	}
}

void ATreasureChestStandard::TreasureEffect_Activate_Implementation()
{
	LootEffectComponent->Activate();
	LootLightComponent->SetIntensity(TreasureLightIntensity);
	SwitchLockRing_Deactivate();
}


void ATreasureChestStandard::InitializeLootFadeEffect_Implementation()
{
	FadeEffectComponent->AddFadeMesh(LootMeshComponent);
	FadeEffectComponent->OnFadeTimelineTickDelegate.AddDynamic(this, &ATreasureChestStandard::LootFade_Update);
	FadeEffectComponent->OnFadeTimelineFinishDelegate.AddDynamic(this, &ATreasureChestStandard::LootFade_Finish);
}

void ATreasureChestStandard::LootFade_Update_Implementation(const float Alpha)
{
	LootLightComponent->SetIntensity(TreasureLightIntensity * Alpha);
}

void ATreasureChestStandard::LootFade_Finish_Implementation()
{
	if (LootMeshComponent)
	{
		LootMeshComponent->Deactivate();
		LootMeshComponent->SetVisibility(false, true);
	}
}

void ATreasureChestStandard::UnlockEffect_Update_Implementation(const float Alpha)
{
	LidMeshComponent->SetRelativeRotation(FRotator(LidUnlockAngle * Alpha, 0, 0));
}

void ATreasureChestStandard::UnlockEffect_Finish_Implementation()
{
	LidMeshComponent->SetRelativeRotation(FRotator(LidUnlockAngle, 0, 0));
	if (UnlockEffectTimeline)
	{
		UnlockEffectTimeline->DestroyComponent();
		UnlockEffectTimeline = nullptr;
	}
}

void ATreasureChestStandard::OpenEffect_Update_Implementation(const float Alpha)
{
	LidMeshComponent->SetRelativeRotation(FRotator(LidOpenAngle * Alpha, 0, 0));
}

void ATreasureChestStandard::OpenEffect_Finish_Implementation()
{
	LidMeshComponent->SetRelativeRotation(FRotator(LidOpenAngle, 0, 0));
	FadeEffectComponent->FadeOut();
	GrantRewards();
	if (OpenEffectTimeline)
	{
		OpenEffectTimeline->DestroyComponent();
		OpenEffectTimeline = nullptr;
	}
}

void ATreasureChestStandard::PlayUnlockEffect_Implementation()
{
	TreasureEffect_Activate();
	UnlockEffectTimeline = NewObject<UTimelineComponent>(this, FName(GetName() + TEXT("_UnlockTimeline")));
	UnlockEffectTimeline->SetComponentTickEnabled(true);
	UnlockEffectTimeline->RegisterComponent();
	UnlockEffectTimeline->AddInterpFloat(UnlockCurve, OnUnlockUpdate);
	UnlockEffectTimeline->SetTimelineFinishedFunc(OnUnlockFinish);
	UnlockEffectTimeline->Play();
}

void ATreasureChestStandard::PlayUnlockForcedEffect_Implementation()
{
	TreasureEffect_Activate();
	LidMeshComponent->SetRelativeRotation(FRotator(LidUnlockAngle, 0, 0));
}

void ATreasureChestStandard::PlayOpenEffect_Implementation(AActor* Player)
{
	TreasureEffect_Activate();
	if (OpenSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, OpenSound, GetActorLocation());
	}
	OpenEffectTimeline = NewObject<UTimelineComponent>(this, FName(GetName() + TEXT("_OpenTimeline")));
	OpenEffectTimeline->SetComponentTickEnabled(true);
	OpenEffectTimeline->RegisterComponent();
	OpenEffectTimeline->AddInterpFloat(OpenCurve, OnOpenUpdate);
	OpenEffectTimeline->SetTimelineFinishedFunc(OnOpenFinish);
	OpenEffectTimeline->Play();
}

void ATreasureChestStandard::PlayOpenForcedEffect_Implementation()
{
	LidMeshComponent->SetRelativeRotation(FRotator(LidOpenAngle, 0, 0));
	SwitchLockRing_Deactivate();
}

void ATreasureChestStandard::GrantRewards_Finish_Implementation()
{
	LootEffectComponent->Deactivate();
	FadeEffectComponent->FadeOut();
}
