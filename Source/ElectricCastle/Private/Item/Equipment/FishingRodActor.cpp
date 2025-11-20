// Copyright Alien Shores


#include "Item/Equipment/FishingRodActor.h"

#include "Item/Equipment/FishingBobActor.h"
#include "CableComponent.h"
#include "Actor/ElectricCastleActorBlueprintFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFishingRodActor::AFishingRodActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CableComponent = CreateDefaultSubobject<UCableComponent>(TEXT("FishingLine"));
	CableComponent->bAttachStart = true;
	CableComponent->SetupAttachment(MeshComponent, RodTipSocket);
}

FVector AFishingRodActor::GetRodTipLocation() const
{
	return GetMesh()->GetSocketLocation(RodTipSocket);
}

FRotator AFishingRodActor::GetRodTipRotation() const
{
	return GetMesh()->GetSocketRotation(RodTipSocket);
}


void AFishingRodActor::BeginPlay()
{
	Super::BeginPlay();
	BobActor = CreateFishingBob();
	CableComponent->SetAttachEndTo(BobActor, BobActor->GetMeshComponentName());
	CableComponent->SetVisibility(false);
	CableComponent->Deactivate();
}

AFishingBobActor* AFishingRodActor::CreateFishingBob()
{
	FTransform Transform;
	Transform.SetLocation(GetRodTipLocation());
	Transform.SetRotation(GetRodTipRotation().Quaternion());
	AFishingBobActor* FishingBob = GetWorld()->SpawnActorDeferred<AFishingBobActor>(
		FishingBobClass,
		Transform,
		this,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	FishingBob->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale, RodTipSocket);
	FishingBob->OnFishingStateChanged.AddDynamic(this, &AFishingRodActor::OnFishingBobStateChanged);
	UGameplayStatics::FinishSpawningActor(FishingBob, Transform);
	UElectricCastleActorBlueprintFunctionLibrary::DisableActor(FishingBob);
	return FishingBob;
}

void AFishingRodActor::Cast(const FVector& Destination)
{
	CableComponent->SetVisibility(true);
	CableComponent->Activate();
	BobActor->SetActorLocation(GetRodTipLocation());
	BobActor->Launch(Destination);
}

void AFishingRodActor::Return()
{
	CableComponent->SetVisibility(false);
	CableComponent->Deactivate();
	if (BobActor)
	{
		BobActor->Return(this, RodTipSocket);
	}
}

void AFishingRodActor::Unequip_Implementation(AActor* InOwner)
{
	Super::Unequip_Implementation(InOwner);
	if (IsValid(BobActor))
	{
		BobActor->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale, RodTipSocket);
		UElectricCastleActorBlueprintFunctionLibrary::DisableActor(BobActor);
		CableComponent->SetVisibility(false);
		CableComponent->Deactivate();
	}
}

AFishingBobActor* AFishingRodActor::GetFishingBob() const
{
	return BobActor;
}

void AFishingRodActor::OnFishingBobStateChanged(EFishingBobState FishingState)
{
	switch (FishingState)
	{
	case EFishingBobState::Bobbing:
		OnFishingBobLandedDelegate.Broadcast();
		break;
	default:
		// DO NOTHING
		break;
	}
}
